#include "../include/AuthenticationManager.h"
#include "../include/SecurityConfig.h"
#include <iostream>
#include <string>

void printMenu() {
    std::cout << "\n=== Secure Login System Demo ===\n";
    std::cout << "1. Register new user\n";
    std::cout << "2. Login\n";
    std::cout << "3. Enable 2FA\n";
    std::cout << "4. Disable 2FA\n";
    std::cout << "5. Change password\n";
    std::cout << "6. View user info\n";
    std::cout << "7. Logout\n";
    std::cout << "8. Save users to file\n";
    std::cout << "9. Load users from file\n";
    std::cout << "10. View security config\n";
    std::cout << "11. Modify security config\n";
    std::cout << "0. Exit\n";
    std::cout << "Choice: ";
}

int main() {
    // Initialize security configuration
    SecurityConfig config;

    // Load configuration from file if it exists
    if (!config.loadFromFile("security_config.txt")) {
        std::cout << "No existing config found, using defaults.\n";
        std::cout << "Creating default security_config.txt...\n";
        config.saveToFile("security_config.txt");
    }

    // Create authentication manager
    AuthenticationManager authManager(config);

    // Try to load existing users
    if (authManager.loadUsers("users.dat")) {
        std::cout << "Loaded existing user database.\n";
    } else {
        std::cout << "Starting with empty user database.\n";
    }

    std::string currentSession;
    std::string currentUser;
    int choice;

    while (true) {
        printMenu();
        std::cin >> choice;
        std::cin.ignore(); // Clear newline

        if (choice == 0) {
            break;
        }

        switch (choice) {
            case 1: { // Register
                std::string username, password, email;
                std::cout << "Username: ";
                std::getline(std::cin, username);
                std::cout << "Password: ";
                std::getline(std::cin, password);
                std::cout << "Email (optional): ";
                std::getline(std::cin, email);

                if (authManager.registerUser(username, password, email)) {
                    std::cout << "✓ User registered successfully!\n";
                } else {
                    std::cout << "✗ Registration failed.\n";
                }
                break;
            }

            case 2: { // Login
                std::string username, password;
                std::cout << "Username: ";
                std::getline(std::cin, username);
                std::cout << "Password: ";
                std::getline(std::cin, password);

                std::string session = authManager.login(username, password);
                if (session.empty()) {
                    std::cout << "✗ Login failed.\n";
                } else if (session.substr(0, 5) == "TEMP_") {
                    std::cout << "2FA is enabled. Enter TOTP code: ";
                    std::string totpCode;
                    std::getline(std::cin, totpCode);

                    if (authManager.verify2FA(username, totpCode, session)) {
                        std::cout << "✓ Login successful with 2FA!\n";
                        currentUser = username;
                        // Note: In a real app, you'd get the new session token
                    } else {
                        std::cout << "✗ Invalid 2FA code.\n";
                    }
                } else {
                    currentSession = session;
                    currentUser = username;
                    std::cout << "✓ Login successful!\n";
                    std::cout << "Session token: " << session << "\n";
                }
                break;
            }

            case 3: { // Enable 2FA
                if (currentUser.empty()) {
                    std::cout << "✗ Please login first.\n";
                    break;
                }

                std::string secret = authManager.enable2FA(currentUser);
                if (!secret.empty()) {
                    std::cout << "✓ 2FA enabled successfully!\n";
                    std::cout << "Secret: " << secret << "\n";
                    std::cout << "\nProvisioning URI (for QR code):\n";
                    std::cout << authManager.get2FAProvisioningURI(currentUser, "SecureAuthDemo") << "\n";
                    std::cout << "\nAdd this to your authenticator app (Google Authenticator, Authy, etc.)\n";
                } else {
                    std::cout << "✗ Failed to enable 2FA.\n";
                }
                break;
            }

            case 4: { // Disable 2FA
                if (currentUser.empty()) {
                    std::cout << "✗ Please login first.\n";
                    break;
                }

                std::string totpCode;
                std::cout << "Enter current TOTP code to verify: ";
                std::getline(std::cin, totpCode);

                if (authManager.disable2FA(currentUser, totpCode)) {
                    std::cout << "✓ 2FA disabled successfully!\n";
                } else {
                    std::cout << "✗ Failed to disable 2FA. Invalid code.\n";
                }
                break;
            }

            case 5: { // Change password
                if (currentUser.empty()) {
                    std::cout << "✗ Please login first.\n";
                    break;
                }

                std::string oldPassword, newPassword;
                std::cout << "Current password: ";
                std::getline(std::cin, oldPassword);
                std::cout << "New password: ";
                std::getline(std::cin, newPassword);

                if (authManager.changePassword(currentUser, oldPassword, newPassword)) {
                    std::cout << "✓ Password changed successfully!\n";
                } else {
                    std::cout << "✗ Failed to change password.\n";
                }
                break;
            }

            case 6: { // View user info
                std::string username;
                std::cout << "Username: ";
                std::getline(std::cin, username);

                std::string info = authManager.getUserInfo(username);
                if (!info.empty()) {
                    std::cout << "\n" << info;
                } else {
                    std::cout << "✗ User not found.\n";
                }
                break;
            }

            case 7: { // Logout
                if (currentSession.empty()) {
                    std::cout << "✗ Not logged in.\n";
                    break;
                }

                if (authManager.logout(currentSession)) {
                    std::cout << "✓ Logged out successfully!\n";
                    currentSession.clear();
                    currentUser.clear();
                } else {
                    std::cout << "✗ Logout failed.\n";
                }
                break;
            }

            case 8: { // Save users
                if (authManager.saveUsers("users.dat")) {
                    std::cout << "✓ Users saved successfully!\n";
                } else {
                    std::cout << "✗ Failed to save users.\n";
                }
                break;
            }

            case 9: { // Load users
                if (authManager.loadUsers("users.dat")) {
                    std::cout << "✓ Users loaded successfully!\n";
                } else {
                    std::cout << "✗ Failed to load users.\n";
                }
                break;
            }

            case 10: { // View config
                std::cout << "\n" << config.toString();
                break;
            }

            case 11: { // Modify config
                std::cout << "Modifying security configuration...\n";
                std::cout << "Min password length (current: " << config.minPasswordLength << "): ";
                std::cin >> config.minPasswordLength;

                std::cout << "Enable 2FA by default? (1=yes, 0=no, current: "
                         << config.enable2FA << "): ";
                int enable2fa;
                std::cin >> enable2fa;
                config.enable2FA = (enable2fa == 1);

                std::cout << "Session timeout in minutes (current: "
                         << config.sessionTimeoutMinutes << "): ";
                std::cin >> config.sessionTimeoutMinutes;

                std::cin.ignore(); // Clear newline

                if (config.saveToFile("security_config.txt")) {
                    std::cout << "✓ Configuration saved!\n";
                    authManager.setConfig(config);
                } else {
                    std::cout << "✗ Failed to save configuration.\n";
                }
                break;
            }

            default:
                std::cout << "Invalid choice.\n";
        }
    }

    // Save users on exit
    authManager.saveUsers("users.dat");
    std::cout << "Goodbye!\n";

    return 0;
}
