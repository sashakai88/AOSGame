#include "../include/User.h"
#include "../include/SecurityConfig.h"
#include "../include/Encryption.h"
#include <sstream>
#include <ctime>

bool User::isLocked() const {
    if (!accountLocked) {
        return false;
    }

    // Check if lockout has expired
    time_t now = std::time(nullptr);
    return now < lockoutUntil;
}

void User::resetFailedAttempts() {
    failedLoginAttempts = 0;
    lastFailedLogin = 0;
}

void User::recordFailedLogin(const SecurityConfig& config) {
    failedLoginAttempts++;
    lastFailedLogin = std::time(nullptr);

    if (config.enableAccountLockout && failedLoginAttempts >= config.maxFailedAttempts) {
        accountLocked = true;
        lockoutUntil = lastFailedLogin + (config.lockoutDurationMinutes * 60);
    }
}

void User::updateLastLogin() {
    lastLogin = std::time(nullptr);
    resetFailedAttempts();
    accountLocked = false;
    lockoutUntil = 0;
}

std::string User::serialize(const std::string& encryptionKey,
                           const std::string& iv) const {
    std::stringstream ss;
    ss << username << "|"
       << passwordHash << "|"
       << salt << "|"
       << totpSecret << "|"
       << (twoFactorEnabled ? "1" : "0") << "|"
       << failedLoginAttempts << "|"
       << lastFailedLogin << "|"
       << accountCreated << "|"
       << lastLogin << "|"
       << (accountLocked ? "1" : "0") << "|"
       << lockoutUntil << "|"
       << email;

    std::string data = ss.str();

    // Encrypt if key is provided
    if (!encryptionKey.empty() && !iv.empty()) {
        data = Encryption::encrypt(data, encryptionKey, iv);
    }

    return data;
}

bool User::deserialize(const std::string& data,
                      const std::string& encryptionKey,
                      const std::string& iv) {
    std::string decrypted = data;

    // Decrypt if key is provided
    if (!encryptionKey.empty() && !iv.empty()) {
        try {
            decrypted = Encryption::decrypt(data, encryptionKey, iv);
        } catch (...) {
            return false;
        }
    }

    std::stringstream ss(decrypted);
    std::string token;
    int field = 0;

    while (std::getline(ss, token, '|')) {
        try {
            switch (field++) {
                case 0: username = token; break;
                case 1: passwordHash = token; break;
                case 2: salt = token; break;
                case 3: totpSecret = token; break;
                case 4: twoFactorEnabled = (token == "1"); break;
                case 5: failedLoginAttempts = std::stoi(token); break;
                case 6: lastFailedLogin = std::stol(token); break;
                case 7: accountCreated = std::stol(token); break;
                case 8: lastLogin = std::stol(token); break;
                case 9: accountLocked = (token == "1"); break;
                case 10: lockoutUntil = std::stol(token); break;
                case 11: email = token; break;
            }
        } catch (...) {
            return false;
        }
    }

    return field >= 11; // Ensure all required fields were read
}
