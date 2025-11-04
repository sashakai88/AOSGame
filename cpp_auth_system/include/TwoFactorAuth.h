#ifndef TWO_FACTOR_AUTH_H
#define TWO_FACTOR_AUTH_H

#include <string>
#include <cstdint>
#include <vector>

/**
 * @brief Time-based One-Time Password (TOTP) implementation for 2FA
 *
 * Implements RFC 6238 TOTP algorithm compatible with Google Authenticator,
 * Authy, and other authenticator apps.
 */
class TwoFactorAuth {
public:
    /**
     * @brief Generate a random secret key for TOTP
     * @param length Length of secret in bytes (default: 20)
     * @return Base32-encoded secret key
     */
    static std::string generateSecret(uint32_t length = 20);

    /**
     * @brief Generate TOTP code for current time
     * @param secret Base32-encoded secret key
     * @param timeStep Time step in seconds (default: 30)
     * @param digits Number of digits in code (default: 6)
     * @return TOTP code as string
     */
    static std::string generateTOTP(const std::string& secret,
                                    uint32_t timeStep = 30,
                                    uint32_t digits = 6);

    /**
     * @brief Verify TOTP code
     * @param secret Base32-encoded secret key
     * @param code Code to verify
     * @param timeStep Time step in seconds (default: 30)
     * @param digits Number of digits in code (default: 6)
     * @param window Time window for validation (default: 1, allows ±1 time step)
     * @return true if code is valid, false otherwise
     */
    static bool verifyTOTP(const std::string& secret,
                          const std::string& code,
                          uint32_t timeStep = 30,
                          uint32_t digits = 6,
                          uint32_t window = 1);

    /**
     * @brief Generate QR code provisioning URI for authenticator apps
     * @param accountName User's account name/email
     * @param issuer Service/application name
     * @param secret Base32-encoded secret key
     * @return otpauth:// URI for QR code generation
     */
    static std::string generateProvisioningURI(const std::string& accountName,
                                               const std::string& issuer,
                                               const std::string& secret);

private:
    /**
     * @brief Get current Unix timestamp
     * @return Current time in seconds since epoch
     */
    static uint64_t getCurrentTimestamp();

    /**
     * @brief Generate HOTP code (HMAC-based OTP)
     * @param secret Decoded secret key
     * @param counter Counter value
     * @param digits Number of digits in code
     * @return HOTP code as string
     */
    static std::string generateHOTP(const std::vector<uint8_t>& secret,
                                    uint64_t counter,
                                    uint32_t digits);

    /**
     * @brief HMAC-SHA1 implementation
     * @param key HMAC key
     * @param message Message to authenticate
     * @return HMAC-SHA1 digest
     */
    static std::vector<uint8_t> hmacSHA1(const std::vector<uint8_t>& key,
                                        const std::vector<uint8_t>& message);

    /**
     * @brief Base32 encode
     * @param data Data to encode
     * @return Base32-encoded string
     */
    static std::string base32Encode(const std::vector<uint8_t>& data);

    /**
     * @brief Base32 decode
     * @param encoded Base32-encoded string
     * @return Decoded data
     */
    static std::vector<uint8_t> base32Decode(const std::string& encoded);
};

#endif // TWO_FACTOR_AUTH_H
