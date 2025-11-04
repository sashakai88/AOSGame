#ifndef SECURITY_CONFIG_H
#define SECURITY_CONFIG_H

#include <string>
#include <cstdint>

/**
 * @brief Configuration class for security settings
 *
 * Allows customization of various security parameters including
 * password requirements, 2FA settings, encryption, and session management.
 */
class SecurityConfig {
public:
    // Password Policy
    uint32_t minPasswordLength = 8;
    uint32_t maxPasswordLength = 128;
    bool requireUppercase = true;
    bool requireLowercase = true;
    bool requireDigits = true;
    bool requireSpecialChars = true;

    // Two-Factor Authentication
    bool enable2FA = false;
    uint32_t totpTimeStep = 30;        // Time step in seconds (typically 30)
    uint32_t totpDigits = 6;           // Number of digits in TOTP code

    // Encryption
    bool enableEncryption = true;
    std::string encryptionAlgorithm = "AES-256";

    // Session Management
    uint32_t sessionTimeoutMinutes = 30;
    uint32_t maxFailedAttempts = 5;
    uint32_t lockoutDurationMinutes = 15;

    // Password Hashing
    uint32_t saltLength = 32;          // Length of salt in bytes
    uint32_t hashIterations = 10000;   // Number of iterations for key derivation

    // Account Security
    bool enableAccountLockout = true;
    bool enableSessionTracking = true;
    bool logFailedAttempts = true;

    SecurityConfig() = default;

    /**
     * @brief Load configuration from file
     * @param filename Path to configuration file
     * @return true if successful, false otherwise
     */
    bool loadFromFile(const std::string& filename);

    /**
     * @brief Save configuration to file
     * @param filename Path to configuration file
     * @return true if successful, false otherwise
     */
    bool saveToFile(const std::string& filename) const;

    /**
     * @brief Validate password against policy
     * @param password Password to validate
     * @return true if password meets requirements, false otherwise
     */
    bool validatePassword(const std::string& password) const;

    /**
     * @brief Get configuration as string for logging
     * @return String representation of configuration
     */
    std::string toString() const;
};

#endif // SECURITY_CONFIG_H
