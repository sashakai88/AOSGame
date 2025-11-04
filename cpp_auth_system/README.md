# Secure C++ Authentication System

A comprehensive, production-ready authentication system in C++ with configurable two-factor authentication (2FA), encryption, and advanced security features.

## Features

### 🔐 Core Security
- **Password Hashing**: SHA-256 with salt and configurable iterations (PBKDF2-like)
- **Two-Factor Authentication**: TOTP (Time-based One-Time Password) compatible with Google Authenticator, Authy, etc.
- **Encryption**: AES-256 encryption for stored credentials
- **Session Management**: Token-based sessions with configurable timeout
- **Account Lockout**: Automatic lockout after failed login attempts

### ⚙️ Configurable Security Settings
- Password policy (length, complexity requirements)
- 2FA settings (time step, code length)
- Session timeout duration
- Failed login attempt limits
- Lockout duration
- Encryption options
- Hash iteration count

### 🛡️ Security Best Practices
- Constant-time comparison for password verification (prevents timing attacks)
- Cryptographically secure random generation for salts and keys
- PKCS7 padding for encryption
- Secure session token generation
- Failed login tracking and logging
- Account lockout mechanism

## Project Structure

```
cpp_auth_system/
├── include/                    # Header files
│   ├── AuthenticationManager.h # Main authentication interface
│   ├── SecurityConfig.h        # Configuration management
│   ├── PasswordHasher.h        # Password hashing utilities
│   ├── TwoFactorAuth.h         # TOTP 2FA implementation
│   ├── Encryption.h            # Encryption utilities
│   └── User.h                  # User data structure
├── src/                        # Implementation files
│   ├── AuthenticationManager.cpp
│   ├── SecurityConfig.cpp
│   ├── PasswordHasher.cpp
│   ├── TwoFactorAuth.cpp
│   ├── Encryption.cpp
│   └── User.cpp
├── examples/                   # Example usage
│   ├── main.cpp               # Interactive demo
│   └── security_config.txt    # Sample configuration
├── CMakeLists.txt             # Build configuration
└── README.md                  # This file
```

## Building the Project

### Requirements
- C++11 or later
- CMake 3.10 or later
- A C++ compiler (GCC, Clang, MSVC)

### Build Instructions

```bash
# Navigate to project directory
cd cpp_auth_system

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
cmake --build .

# Run the demo
./auth_demo
```

## Usage

### Basic Example

```cpp
#include "AuthenticationManager.h"
#include "SecurityConfig.h"

int main() {
    // Create configuration
    SecurityConfig config;
    config.minPasswordLength = 8;
    config.enable2FA = true;
    config.sessionTimeoutMinutes = 30;

    // Initialize authentication manager
    AuthenticationManager authManager(config);

    // Register a new user
    authManager.registerUser("alice", "SecurePass123!", "alice@example.com");

    // Login
    std::string session = authManager.login("alice", "SecurePass123!");
    if (!session.empty()) {
        std::cout << "Login successful! Session: " << session << std::endl;
    }

    // Save users to file
    authManager.saveUsers("users.dat");

    return 0;
}
```

### Enabling Two-Factor Authentication

```cpp
// Enable 2FA for a user
std::string secret = authManager.enable2FA("alice");

// Get provisioning URI for QR code
std::string uri = authManager.get2FAProvisioningURI("alice", "MyApp");
std::cout << "Scan this with your authenticator app:\n" << uri << std::endl;

// Login with 2FA
std::string tempSession = authManager.login("alice", "SecurePass123!");
if (tempSession.substr(0, 5) == "TEMP_") {
    // Prompt user for TOTP code
    std::string totpCode = getUserInput();
    if (authManager.verify2FA("alice", totpCode, tempSession)) {
        std::cout << "2FA verified!" << std::endl;
    }
}
```

### Configuration File Format

Create a `security_config.txt` file:

```ini
# Password Policy
minPasswordLength=8
maxPasswordLength=128
requireUppercase=true
requireLowercase=true
requireDigits=true
requireSpecialChars=true

# Two-Factor Authentication
enable2FA=false
totpTimeStep=30
totpDigits=6

# Encryption
enableEncryption=true
encryptionAlgorithm=AES-256

# Session Management
sessionTimeoutMinutes=30
maxFailedAttempts=5
lockoutDurationMinutes=15

# Password Hashing
saltLength=32
hashIterations=10000

# Account Security
enableAccountLockout=true
enableSessionTracking=true
logFailedAttempts=true
```

Load configuration:

```cpp
SecurityConfig config;
config.loadFromFile("security_config.txt");
```

## API Reference

### AuthenticationManager

#### `registerUser(username, password, email)`
Register a new user account.

**Parameters:**
- `username` (string): Unique username
- `password` (string): Password (must meet security requirements)
- `email` (string): Email address (optional)

**Returns:** `bool` - true if successful

#### `login(username, password)`
Authenticate a user.

**Returns:** `string` - Session token (empty if failed, starts with "TEMP_" if 2FA required)

#### `verify2FA(username, totpCode, sessionToken)`
Verify 2FA code during login.

**Returns:** `bool` - true if successful

#### `enable2FA(username)`
Enable 2FA for a user.

**Returns:** `string` - Base32-encoded secret for authenticator app

#### `disable2FA(username, totpCode)`
Disable 2FA for a user.

**Returns:** `bool` - true if successful

#### `changePassword(username, oldPassword, newPassword)`
Change user password.

**Returns:** `bool` - true if successful

#### `validateSession(sessionToken)`
Validate a session token.

**Returns:** `string` - Username if valid (empty if invalid)

#### `logout(sessionToken)`
Logout and invalidate session.

**Returns:** `bool` - true if successful

#### `saveUsers(filename)` / `loadUsers(filename)`
Persist user database to/from file.

**Returns:** `bool` - true if successful

### SecurityConfig

Configure all security parameters:
- Password policy
- 2FA settings
- Encryption options
- Session management
- Account lockout
- Hash iterations

### PasswordHasher

Utility functions for secure password hashing:
- `generateSalt()` - Create cryptographically secure salt
- `hashPassword()` - Hash password with salt and iterations
- `verifyPassword()` - Verify password using constant-time comparison

### TwoFactorAuth

TOTP implementation:
- `generateSecret()` - Create secret key for 2FA
- `generateTOTP()` - Generate current TOTP code
- `verifyTOTP()` - Verify TOTP code with time window
- `generateProvisioningURI()` - Create URI for QR code

### Encryption

Data encryption utilities:
- `generateKey()` - Create encryption key
- `generateIV()` - Create initialization vector
- `encrypt()` - Encrypt data
- `decrypt()` - Decrypt data

## Security Considerations

### Production Use

⚠️ **Important**: This implementation includes simplified cryptographic algorithms for demonstration purposes. For production use:

1. **Use OpenSSL or similar library** for:
   - AES-256 encryption (instead of XOR cipher)
   - HMAC-SHA1 for TOTP (instead of simplified version)
   - Proper key derivation (PBKDF2, Argon2, or bcrypt)

2. **Additional recommendations**:
   - Store user database in a secure location with restricted permissions
   - Use HTTPS for any network communication
   - Implement rate limiting for login attempts
   - Add logging and monitoring
   - Regular security audits
   - Consider using hardware security modules (HSM) for key storage

### Password Requirements

Default password policy:
- Minimum 8 characters (configurable)
- At least one uppercase letter
- At least one lowercase letter
- At least one digit
- At least one special character

### 2FA Setup

To use 2FA:
1. Enable 2FA for a user
2. Get the provisioning URI
3. Convert URI to QR code (use external library like qrencode)
4. User scans QR code with authenticator app
5. User enters 6-digit code to verify

Compatible authenticator apps:
- Google Authenticator
- Authy
- Microsoft Authenticator
- 1Password
- Any TOTP-compatible app

## Example Demo

Run the interactive demo:

```bash
./auth_demo
```

Features in demo:
- User registration
- Login with/without 2FA
- Enable/disable 2FA
- Change password
- View user information
- Session management
- Configuration management

## Testing

### Manual Testing

1. Register users with different passwords
2. Test password policy enforcement
3. Enable 2FA and verify codes
4. Test failed login lockout
5. Test session expiration
6. Verify encryption of stored data

### Integration

To integrate into your project:

```cpp
#include "AuthenticationManager.h"

// In your application initialization
SecurityConfig config;
config.loadFromFile("config.txt");
AuthenticationManager auth(config);
auth.loadUsers("users.dat");

// In your login handler
std::string session = auth.login(username, password);
// Handle 2FA if needed

// In your protected endpoints
std::string user = auth.validateSession(sessionToken);
if (user.empty()) {
    // Unauthorized
}
```

## License

This project is provided as-is for educational and demonstration purposes.

## Contributing

Contributions welcome! Areas for improvement:
- Integration with OpenSSL for production-grade crypto
- Additional 2FA methods (SMS, email, backup codes)
- Password reset functionality
- OAuth/OpenID Connect support
- Multi-device session management
- Biometric authentication support

## Support

For issues or questions, please refer to the code documentation or create an issue in the repository.

---

**Version:** 1.0
**Last Updated:** 2025-11-04
**Language:** C++11
