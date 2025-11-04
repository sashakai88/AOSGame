#ifndef USER_H
#define USER_H

#include <string>
#include <ctime>
#include <vector>

/**
 * @brief User account data structure
 *
 * Stores user credentials and security information
 */
class User {
public:
    std::string username;
    std::string passwordHash;
    std::string salt;
    std::string totpSecret;           // Base32-encoded TOTP secret
    bool twoFactorEnabled;
    uint32_t failedLoginAttempts;
    time_t lastFailedLogin;
    time_t accountCreated;
    time_t lastLogin;
    bool accountLocked;
    time_t lockoutUntil;
    std::string email;
    std::vector<std::string> sessions; // Active session tokens

    User()
        : twoFactorEnabled(false),
          failedLoginAttempts(0),
          lastFailedLogin(0),
          accountCreated(std::time(nullptr)),
          lastLogin(0),
          accountLocked(false),
          lockoutUntil(0) {}

    User(const std::string& uname, const std::string& email = "")
        : username(uname),
          email(email),
          twoFactorEnabled(false),
          failedLoginAttempts(0),
          lastFailedLogin(0),
          accountCreated(std::time(nullptr)),
          lastLogin(0),
          accountLocked(false),
          lockoutUntil(0) {}

    /**
     * @brief Check if account is currently locked
     * @return true if locked, false otherwise
     */
    bool isLocked() const;

    /**
     * @brief Reset failed login attempts
     */
    void resetFailedAttempts();

    /**
     * @brief Increment failed login attempts
     * @param config Security configuration
     */
    void recordFailedLogin(const class SecurityConfig& config);

    /**
     * @brief Update last login time
     */
    void updateLastLogin();

    /**
     * @brief Serialize user data to string (for storage)
     * @param encryptionKey Encryption key (if encryption is enabled)
     * @param iv Initialization vector
     * @return Serialized user data
     */
    std::string serialize(const std::string& encryptionKey = "",
                         const std::string& iv = "") const;

    /**
     * @brief Deserialize user data from string
     * @param data Serialized user data
     * @param encryptionKey Encryption key (if encryption is enabled)
     * @param iv Initialization vector
     * @return true if successful, false otherwise
     */
    bool deserialize(const std::string& data,
                    const std::string& encryptionKey = "",
                    const std::string& iv = "");
};

#endif // USER_H
