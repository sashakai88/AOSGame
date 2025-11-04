#ifndef PASSWORD_HASHER_H
#define PASSWORD_HASHER_H

#include <string>
#include <vector>
#include <cstdint>

/**
 * @brief Password hashing utilities using SHA-256 with salt and iterations
 *
 * Implements secure password hashing with:
 * - Random salt generation
 * - Configurable iteration count (PBKDF2-like)
 * - Constant-time comparison to prevent timing attacks
 */
class PasswordHasher {
public:
    /**
     * @brief Generate cryptographically secure random salt
     * @param length Length of salt in bytes
     * @return Salt as hex string
     */
    static std::string generateSalt(uint32_t length = 32);

    /**
     * @brief Hash password with salt and iterations
     * @param password Plain text password
     * @param salt Salt value (hex string)
     * @param iterations Number of hashing iterations
     * @return Hashed password as hex string
     */
    static std::string hashPassword(const std::string& password,
                                    const std::string& salt,
                                    uint32_t iterations = 10000);

    /**
     * @brief Verify password against stored hash
     * @param password Plain text password to verify
     * @param storedHash Stored hash value
     * @param salt Salt used for hashing
     * @param iterations Number of iterations used
     * @return true if password matches, false otherwise
     */
    static bool verifyPassword(const std::string& password,
                              const std::string& storedHash,
                              const std::string& salt,
                              uint32_t iterations = 10000);

    /**
     * @brief Constant-time string comparison to prevent timing attacks
     * @param a First string
     * @param b Second string
     * @return true if strings are equal, false otherwise
     */
    static bool constantTimeCompare(const std::string& a, const std::string& b);

private:
    /**
     * @brief SHA-256 hashing function
     * @param data Data to hash
     * @return Hash as hex string
     */
    static std::string sha256(const std::string& data);

    /**
     * @brief Convert bytes to hex string
     * @param data Byte vector
     * @return Hex string
     */
    static std::string bytesToHex(const std::vector<uint8_t>& data);
};

#endif // PASSWORD_HASHER_H
