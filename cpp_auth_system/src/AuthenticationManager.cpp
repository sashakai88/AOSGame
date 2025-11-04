#include "../include/AuthenticationManager.h"
#include "../include/PasswordHasher.h"
#include "../include/TwoFactorAuth.h"
#include "../include/Encryption.h"
#include <fstream>
#include <sstream>
#include <random>
#include <iostream>

AuthenticationManager::AuthenticationManager(const SecurityConfig& config)
    : config_(config) {
    if (config_.enableEncryption) {
        encryptionKey_ = Encryption::generateKey(32);
        encryptionIV_ = Encryption::generateIV(16);
    }
}

bool AuthenticationManager::registerUser(const std::string& username,
                                        const std::string& password,
                                        const std::string& email) {
    // Check if user already exists
    if (users_.find(username) != users_.end()) {
        std::cerr << "Error: User '" << username << "' already exists\n";
        return false;
    }

    // Validate password
    if (!config_.validatePassword(password)) {
        std::cerr << "Error: Password does not meet security requirements\n";
        return false;
    }

    // Create new user
    User user(username, email);

    // Generate salt and hash password
    user.salt = PasswordHasher::generateSalt(config_.saltLength);
    user.passwordHash = PasswordHasher::hashPassword(password, user.salt, config_.hashIterations);

    // Enable 2FA if configured
    if (config_.enable2FA) {
        user.totpSecret = TwoFactorAuth::generateSecret();
        user.twoFactorEnabled = true;
    }

    users_[username] = user;
    return true;
}

std::string AuthenticationManager::login(const std::string& username,
                                        const std::string& password) {
    auto it = users_.find(username);
    if (it == users_.end()) {
        std::cerr << "Error: User '" << username << "' not found\n";
        return "";
    }

    User& user = it->second;

    // Check if account is locked
    if (isAccountLocked(user)) {
        std::cerr << "Error: Account is locked. Try again later.\n";
        return "";
    }

    // Verify password
    if (!PasswordHasher::verifyPassword(password, user.passwordHash, user.salt, config_.hashIterations)) {
        user.recordFailedLogin(config_);
        if (config_.logFailedAttempts) {
            std::cerr << "Failed login attempt for user: " << username << "\n";
        }
        return "";
    }

    // If 2FA is enabled, return temporary session token
    if (user.twoFactorEnabled) {
        std::string tempToken = "TEMP_" + generateSessionToken();
        sessions_[tempToken] = username;
        sessionExpiry_[tempToken] = std::time(nullptr) + 300; // 5 minutes for 2FA
        return tempToken;
    }

    // Update login info
    user.updateLastLogin();

    // Create session
    std::string sessionToken = generateSessionToken();
    sessions_[sessionToken] = username;
    sessionExpiry_[sessionToken] = std::time(nullptr) + (config_.sessionTimeoutMinutes * 60);
    user.sessions.push_back(sessionToken);

    return sessionToken;
}

bool AuthenticationManager::verify2FA(const std::string& username,
                                     const std::string& totpCode,
                                     const std::string& sessionToken) {
    // Verify it's a temporary session token
    if (sessionToken.substr(0, 5) != "TEMP_") {
        return false;
    }

    auto it = users_.find(username);
    if (it == users_.end()) {
        return false;
    }

    User& user = it->second;

    // Verify TOTP code
    if (!TwoFactorAuth::verifyTOTP(user.totpSecret, totpCode,
                                   config_.totpTimeStep, config_.totpDigits)) {
        user.recordFailedLogin(config_);
        return false;
    }

    // Remove temporary token
    sessions_.erase(sessionToken);
    sessionExpiry_.erase(sessionToken);

    // Update login info
    user.updateLastLogin();

    // Create permanent session
    std::string newSessionToken = generateSessionToken();
    sessions_[newSessionToken] = username;
    sessionExpiry_[newSessionToken] = std::time(nullptr) + (config_.sessionTimeoutMinutes * 60);
    user.sessions.push_back(newSessionToken);

    return true;
}

std::string AuthenticationManager::enable2FA(const std::string& username) {
    auto it = users_.find(username);
    if (it == users_.end()) {
        return "";
    }

    User& user = it->second;

    if (user.twoFactorEnabled) {
        std::cerr << "2FA is already enabled for this user\n";
        return user.totpSecret;
    }

    user.totpSecret = TwoFactorAuth::generateSecret();
    user.twoFactorEnabled = true;

    return user.totpSecret;
}

bool AuthenticationManager::disable2FA(const std::string& username,
                                      const std::string& totpCode) {
    auto it = users_.find(username);
    if (it == users_.end()) {
        return false;
    }

    User& user = it->second;

    if (!user.twoFactorEnabled) {
        return true;
    }

    // Verify TOTP code before disabling
    if (!TwoFactorAuth::verifyTOTP(user.totpSecret, totpCode,
                                   config_.totpTimeStep, config_.totpDigits)) {
        return false;
    }

    user.twoFactorEnabled = false;
    user.totpSecret = "";

    return true;
}

std::string AuthenticationManager::get2FAProvisioningURI(const std::string& username,
                                                         const std::string& issuer) {
    auto it = users_.find(username);
    if (it == users_.end() || !it->second.twoFactorEnabled) {
        return "";
    }

    return TwoFactorAuth::generateProvisioningURI(username, issuer, it->second.totpSecret);
}

std::string AuthenticationManager::validateSession(const std::string& sessionToken) {
    cleanupExpiredSessions();

    auto it = sessions_.find(sessionToken);
    if (it == sessions_.end()) {
        return "";
    }

    // Check expiry
    auto expiryIt = sessionExpiry_.find(sessionToken);
    if (expiryIt != sessionExpiry_.end() && std::time(nullptr) > expiryIt->second) {
        sessions_.erase(it);
        sessionExpiry_.erase(expiryIt);
        return "";
    }

    return it->second;
}

bool AuthenticationManager::logout(const std::string& sessionToken) {
    auto it = sessions_.find(sessionToken);
    if (it == sessions_.end()) {
        return false;
    }

    std::string username = it->second;
    sessions_.erase(it);
    sessionExpiry_.erase(sessionToken);

    // Remove from user's session list
    auto userIt = users_.find(username);
    if (userIt != users_.end()) {
        auto& userSessions = userIt->second.sessions;
        userSessions.erase(std::remove(userSessions.begin(), userSessions.end(), sessionToken),
                          userSessions.end());
    }

    return true;
}

bool AuthenticationManager::changePassword(const std::string& username,
                                          const std::string& oldPassword,
                                          const std::string& newPassword) {
    auto it = users_.find(username);
    if (it == users_.end()) {
        return false;
    }

    User& user = it->second;

    // Verify old password
    if (!PasswordHasher::verifyPassword(oldPassword, user.passwordHash,
                                       user.salt, config_.hashIterations)) {
        return false;
    }

    // Validate new password
    if (!config_.validatePassword(newPassword)) {
        std::cerr << "Error: New password does not meet security requirements\n";
        return false;
    }

    // Generate new salt and hash
    user.salt = PasswordHasher::generateSalt(config_.saltLength);
    user.passwordHash = PasswordHasher::hashPassword(newPassword, user.salt, config_.hashIterations);

    return true;
}

bool AuthenticationManager::deleteUser(const std::string& username,
                                      const std::string& password) {
    auto it = users_.find(username);
    if (it == users_.end()) {
        return false;
    }

    // Verify password
    if (!PasswordHasher::verifyPassword(password, it->second.passwordHash,
                                       it->second.salt, config_.hashIterations)) {
        return false;
    }

    // Logout all sessions
    for (const auto& session : it->second.sessions) {
        sessions_.erase(session);
        sessionExpiry_.erase(session);
    }

    users_.erase(it);
    return true;
}

std::string AuthenticationManager::getUserInfo(const std::string& username) {
    auto it = users_.find(username);
    if (it == users_.end()) {
        return "";
    }

    const User& user = it->second;
    std::stringstream ss;
    ss << "Username: " << user.username << "\n";
    ss << "Email: " << user.email << "\n";
    ss << "Account Created: " << std::ctime(&user.accountCreated);
    ss << "Last Login: " << (user.lastLogin ? std::ctime(&user.lastLogin) : "Never\n");
    ss << "2FA Enabled: " << (user.twoFactorEnabled ? "Yes" : "No") << "\n";
    ss << "Account Locked: " << (user.isLocked() ? "Yes" : "No") << "\n";
    ss << "Active Sessions: " << user.sessions.size() << "\n";

    return ss.str();
}

bool AuthenticationManager::loadUsers(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    users_.clear();
    std::string line;

    // Read encryption key and IV if present
    std::getline(file, line);
    if (line.substr(0, 4) == "KEY:") {
        encryptionKey_ = line.substr(4);
        std::getline(file, line);
        if (line.substr(0, 3) == "IV:") {
            encryptionIV_ = line.substr(3);
        }
    }

    // Read users
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        User user;
        if (config_.enableEncryption) {
            if (!user.deserialize(line, encryptionKey_, encryptionIV_)) {
                continue;
            }
        } else {
            if (!user.deserialize(line)) {
                continue;
            }
        }

        users_[user.username] = user;
    }

    file.close();
    return true;
}

bool AuthenticationManager::saveUsers(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // Write encryption key and IV if encryption is enabled
    if (config_.enableEncryption) {
        file << "KEY:" << encryptionKey_ << "\n";
        file << "IV:" << encryptionIV_ << "\n";
    }

    // Write users
    for (const auto& pair : users_) {
        if (config_.enableEncryption) {
            file << pair.second.serialize(encryptionKey_, encryptionIV_) << "\n";
        } else {
            file << pair.second.serialize() << "\n";
        }
    }

    file.close();
    return true;
}

std::string AuthenticationManager::generateSessionToken() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    const char* hex = "0123456789abcdef";
    std::string token;
    for (int i = 0; i < 64; ++i) {
        token += hex[dis(gen)];
    }

    return token;
}

void AuthenticationManager::cleanupExpiredSessions() {
    time_t now = std::time(nullptr);
    std::vector<std::string> expiredTokens;

    for (const auto& pair : sessionExpiry_) {
        if (now > pair.second) {
            expiredTokens.push_back(pair.first);
        }
    }

    for (const auto& token : expiredTokens) {
        std::string username = sessions_[token];
        sessions_.erase(token);
        sessionExpiry_.erase(token);

        // Remove from user's session list
        auto it = users_.find(username);
        if (it != users_.end()) {
            auto& userSessions = it->second.sessions;
            userSessions.erase(std::remove(userSessions.begin(), userSessions.end(), token),
                              userSessions.end());
        }
    }
}

bool AuthenticationManager::isAccountLocked(User& user) {
    if (!config_.enableAccountLockout) {
        return false;
    }

    if (user.isLocked()) {
        return true;
    }

    // Unlock if lockout period has expired
    if (user.accountLocked && !user.isLocked()) {
        user.accountLocked = false;
        user.lockoutUntil = 0;
        user.resetFailedAttempts();
    }

    return false;
}
