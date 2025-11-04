#ifndef AUTHENTICATION_MANAGER_H
#define AUTHENTICATION_MANAGER_H

#include "SecurityConfig.h"
#include "User.h"
#include <string>
#include <map>
#include <memory>

/**
 * @brief Main authentication manager class
 *
 * Handles user registration, login, 2FA setup, and session management
 * with configurable security options.
 */
class AuthenticationManager {
public:
    /**
     * @brief Constructor
     * @param config Security configuration
     */
    explicit AuthenticationManager(const SecurityConfig& config = SecurityConfig());

    /**
     * @brief Register a new user
     * @param username Username
     * @param password Password
     * @param email Email address (optional)
     * @return true if successful, false otherwise
     */
    bool registerUser(const std::string& username,
                     const std::string& password,
                     const std::string& email = "");

    /**
     * @brief Authenticate user with username and password
     * @param username Username
     * @param password Password
     * @return Session token if successful, empty string otherwise
     */
    std::string login(const std::string& username, const std::string& password);

    /**
     * @brief Complete login with 2FA code
     * @param username Username
     * @param totpCode TOTP code from authenticator app
     * @param sessionToken Temporary session token from login()
     * @return true if successful, false otherwise
     */
    bool verify2FA(const std::string& username,
                  const std::string& totpCode,
                  const std::string& sessionToken);

    /**
     * @brief Enable 2FA for user
     * @param username Username
     * @return Base32-encoded secret for QR code generation, empty if failed
     */
    std::string enable2FA(const std::string& username);

    /**
     * @brief Disable 2FA for user
     * @param username Username
     * @param totpCode Current TOTP code to verify
     * @return true if successful, false otherwise
     */
    bool disable2FA(const std::string& username, const std::string& totpCode);

    /**
     * @brief Get provisioning URI for 2FA setup (for QR code)
     * @param username Username
     * @param issuer Application/service name
     * @return otpauth:// URI
     */
    std::string get2FAProvisioningURI(const std::string& username,
                                      const std::string& issuer = "SecureAuthApp");

    /**
     * @brief Validate session token
     * @param sessionToken Session token
     * @return Username if valid, empty string otherwise
     */
    std::string validateSession(const std::string& sessionToken);

    /**
     * @brief Logout user
     * @param sessionToken Session token
     * @return true if successful, false otherwise
     */
    bool logout(const std::string& sessionToken);

    /**
     * @brief Change user password
     * @param username Username
     * @param oldPassword Current password
     * @param newPassword New password
     * @return true if successful, false otherwise
     */
    bool changePassword(const std::string& username,
                       const std::string& oldPassword,
                       const std::string& newPassword);

    /**
     * @brief Delete user account
     * @param username Username
     * @param password Password for verification
     * @return true if successful, false otherwise
     */
    bool deleteUser(const std::string& username, const std::string& password);

    /**
     * @brief Get user information (excluding sensitive data)
     * @param username Username
     * @return User info as string
     */
    std::string getUserInfo(const std::string& username);

    /**
     * @brief Load users from file
     * @param filename Path to user database file
     * @return true if successful, false otherwise
     */
    bool loadUsers(const std::string& filename);

    /**
     * @brief Save users to file
     * @param filename Path to user database file
     * @return true if successful, false otherwise
     */
    bool saveUsers(const std::string& filename) const;

    /**
     * @brief Get security configuration
     * @return Current security configuration
     */
    const SecurityConfig& getConfig() const { return config_; }

    /**
     * @brief Update security configuration
     * @param config New security configuration
     */
    void setConfig(const SecurityConfig& config) { config_ = config; }

private:
    SecurityConfig config_;
    std::map<std::string, User> users_;
    std::map<std::string, std::string> sessions_; // sessionToken -> username
    std::map<std::string, time_t> sessionExpiry_;   // sessionToken -> expiry time
    std::string encryptionKey_;
    std::string encryptionIV_;

    /**
     * @brief Generate session token
     * @return Random session token
     */
    std::string generateSessionToken();

    /**
     * @brief Clean up expired sessions
     */
    void cleanupExpiredSessions();

    /**
     * @brief Check if user account is locked
     * @param user User object
     * @return true if locked, false otherwise
     */
    bool isAccountLocked(User& user);
};

#endif // AUTHENTICATION_MANAGER_H
