#include "../include/Encryption.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <algorithm>

std::string Encryption::generateKey(uint32_t length) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    std::vector<uint8_t> key(length);
    for (uint32_t i = 0; i < length; ++i) {
        key[i] = static_cast<uint8_t>(dis(gen));
    }

    return bytesToHex(key);
}

std::string Encryption::generateIV(uint32_t length) {
    return generateKey(length);
}

std::string Encryption::encrypt(const std::string& plaintext,
                               const std::string& key,
                               const std::string& iv) {
    std::vector<uint8_t> keyBytes = hexToBytes(key);
    std::vector<uint8_t> ivBytes = hexToBytes(iv);
    std::vector<uint8_t> plaintextBytes(plaintext.begin(), plaintext.end());

    // Add PKCS7 padding
    plaintextBytes = addPadding(plaintextBytes, 16);

    // Simple XOR cipher (placeholder for real AES)
    // In production, use OpenSSL's AES implementation
    std::vector<uint8_t> encrypted = xorCipher(plaintextBytes, keyBytes);

    // XOR with IV for first block (CBC mode simulation)
    for (size_t i = 0; i < std::min(ivBytes.size(), encrypted.size()); ++i) {
        encrypted[i] ^= ivBytes[i % ivBytes.size()];
    }

    return bytesToHex(encrypted);
}

std::string Encryption::decrypt(const std::string& ciphertext,
                               const std::string& key,
                               const std::string& iv) {
    std::vector<uint8_t> keyBytes = hexToBytes(key);
    std::vector<uint8_t> ivBytes = hexToBytes(iv);
    std::vector<uint8_t> ciphertextBytes = hexToBytes(ciphertext);

    // XOR with IV for first block (CBC mode simulation)
    for (size_t i = 0; i < std::min(ivBytes.size(), ciphertextBytes.size()); ++i) {
        ciphertextBytes[i] ^= ivBytes[i % ivBytes.size()];
    }

    // Simple XOR cipher (placeholder for real AES)
    std::vector<uint8_t> decrypted = xorCipher(ciphertextBytes, keyBytes);

    // Remove PKCS7 padding
    decrypted = removePadding(decrypted);

    return std::string(decrypted.begin(), decrypted.end());
}

std::vector<uint8_t> Encryption::xorCipher(const std::vector<uint8_t>& data,
                                          const std::vector<uint8_t>& key) {
    std::vector<uint8_t> result(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        result[i] = data[i] ^ key[i % key.size()];
    }
    return result;
}

std::vector<uint8_t> Encryption::addPadding(const std::vector<uint8_t>& data,
                                           uint32_t blockSize) {
    uint32_t paddingLength = blockSize - (data.size() % blockSize);
    std::vector<uint8_t> padded = data;
    for (uint32_t i = 0; i < paddingLength; ++i) {
        padded.push_back(static_cast<uint8_t>(paddingLength));
    }
    return padded;
}

std::vector<uint8_t> Encryption::removePadding(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        return data;
    }

    uint8_t paddingLength = data.back();
    if (paddingLength > data.size() || paddingLength == 0) {
        return data;
    }

    // Verify padding
    for (size_t i = data.size() - paddingLength; i < data.size(); ++i) {
        if (data[i] != paddingLength) {
            return data;
        }
    }

    return std::vector<uint8_t>(data.begin(), data.end() - paddingLength);
}

std::vector<uint8_t> Encryption::hexToBytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

std::string Encryption::bytesToHex(const std::vector<uint8_t>& data) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (uint8_t byte : data) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}
