#include "../include/TwoFactorAuth.h"
#include <random>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>

std::string TwoFactorAuth::generateSecret(uint32_t length) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    std::vector<uint8_t> secret(length);
    for (uint32_t i = 0; i < length; ++i) {
        secret[i] = static_cast<uint8_t>(dis(gen));
    }

    return base32Encode(secret);
}

std::string TwoFactorAuth::generateTOTP(const std::string& secret,
                                       uint32_t timeStep,
                                       uint32_t digits) {
    uint64_t timestamp = getCurrentTimestamp();
    uint64_t counter = timestamp / timeStep;

    std::vector<uint8_t> decodedSecret = base32Decode(secret);
    return generateHOTP(decodedSecret, counter, digits);
}

bool TwoFactorAuth::verifyTOTP(const std::string& secret,
                              const std::string& code,
                              uint32_t timeStep,
                              uint32_t digits,
                              uint32_t window) {
    uint64_t timestamp = getCurrentTimestamp();
    uint64_t counter = timestamp / timeStep;

    std::vector<uint8_t> decodedSecret = base32Decode(secret);

    // Check current time window and adjacent windows
    for (int64_t i = -static_cast<int64_t>(window); i <= static_cast<int64_t>(window); ++i) {
        std::string expectedCode = generateHOTP(decodedSecret, counter + i, digits);
        if (expectedCode == code) {
            return true;
        }
    }

    return false;
}

std::string TwoFactorAuth::generateProvisioningURI(const std::string& accountName,
                                                   const std::string& issuer,
                                                   const std::string& secret) {
    std::stringstream ss;
    ss << "otpauth://totp/" << issuer << ":" << accountName
       << "?secret=" << secret
       << "&issuer=" << issuer
       << "&algorithm=SHA1"
       << "&digits=6"
       << "&period=30";
    return ss.str();
}

uint64_t TwoFactorAuth::getCurrentTimestamp() {
    return static_cast<uint64_t>(std::time(nullptr));
}

std::string TwoFactorAuth::generateHOTP(const std::vector<uint8_t>& secret,
                                       uint64_t counter,
                                       uint32_t digits) {
    // Convert counter to bytes (big-endian)
    std::vector<uint8_t> counterBytes(8);
    for (int i = 7; i >= 0; --i) {
        counterBytes[i] = counter & 0xFF;
        counter >>= 8;
    }

    // HMAC-SHA1
    std::vector<uint8_t> hmac = hmacSHA1(secret, counterBytes);

    // Dynamic truncation
    uint32_t offset = hmac[19] & 0x0F;
    uint32_t code = ((hmac[offset] & 0x7F) << 24) |
                    ((hmac[offset + 1] & 0xFF) << 16) |
                    ((hmac[offset + 2] & 0xFF) << 8) |
                    (hmac[offset + 3] & 0xFF);

    // Generate digits
    uint32_t modulo = 1;
    for (uint32_t i = 0; i < digits; ++i) {
        modulo *= 10;
    }
    code %= modulo;

    // Format with leading zeros
    std::stringstream ss;
    ss << std::setw(digits) << std::setfill('0') << code;
    return ss.str();
}

std::vector<uint8_t> TwoFactorAuth::hmacSHA1(const std::vector<uint8_t>& key,
                                            const std::vector<uint8_t>& message) {
    // Simplified HMAC-SHA1 implementation
    // In production, use OpenSSL or another crypto library
    const size_t blockSize = 64;
    const size_t outputSize = 20;

    std::vector<uint8_t> k = key;
    if (k.size() > blockSize) {
        // Hash key if too long (simplified - should use SHA1)
        k.resize(blockSize, 0);
    } else if (k.size() < blockSize) {
        k.resize(blockSize, 0);
    }

    // Create inner and outer padded keys
    std::vector<uint8_t> ipad(blockSize), opad(blockSize);
    for (size_t i = 0; i < blockSize; ++i) {
        ipad[i] = k[i] ^ 0x36;
        opad[i] = k[i] ^ 0x5C;
    }

    // Simplified SHA1-like hash for demonstration
    // This is NOT cryptographically secure - use OpenSSL in production
    std::vector<uint8_t> innerHash(outputSize);
    for (size_t i = 0; i < message.size() && i < outputSize; ++i) {
        innerHash[i] = message[i] ^ ipad[i % blockSize];
    }

    std::vector<uint8_t> result(outputSize);
    for (size_t i = 0; i < outputSize; ++i) {
        result[i] = innerHash[i] ^ opad[i % blockSize];
    }

    return result;
}

std::string TwoFactorAuth::base32Encode(const std::vector<uint8_t>& data) {
    const char* base32Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    std::string result;

    uint32_t buffer = 0;
    int bitsLeft = 0;

    for (uint8_t byte : data) {
        buffer = (buffer << 8) | byte;
        bitsLeft += 8;

        while (bitsLeft >= 5) {
            result += base32Chars[(buffer >> (bitsLeft - 5)) & 0x1F];
            bitsLeft -= 5;
        }
    }

    if (bitsLeft > 0) {
        result += base32Chars[(buffer << (5 - bitsLeft)) & 0x1F];
    }

    // Add padding
    while (result.length() % 8 != 0) {
        result += '=';
    }

    return result;
}

std::vector<uint8_t> TwoFactorAuth::base32Decode(const std::string& encoded) {
    const char* base32Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    std::vector<uint8_t> result;

    uint32_t buffer = 0;
    int bitsLeft = 0;

    for (char c : encoded) {
        if (c == '=') break;

        const char* pos = std::strchr(base32Chars, std::toupper(c));
        if (!pos) continue;

        buffer = (buffer << 5) | (pos - base32Chars);
        bitsLeft += 5;

        if (bitsLeft >= 8) {
            result.push_back((buffer >> (bitsLeft - 8)) & 0xFF);
            bitsLeft -= 8;
        }
    }

    return result;
}
