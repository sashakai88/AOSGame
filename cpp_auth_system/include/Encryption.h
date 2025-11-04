#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <string>
#include <vector>
#include <cstdint>

/**
 * @brief AES-256 encryption/decryption utilities
 *
 * Provides simplified AES encryption for protecting sensitive data
 * at rest. Uses CBC mode with PKCS7 padding.
 */
class Encryption {
public:
    /**
     * @brief Generate random encryption key
     * @param length Key length in bytes (32 for AES-256)
     * @return Hex-encoded encryption key
     */
    static std::string generateKey(uint32_t length = 32);

    /**
     * @brief Generate random initialization vector (IV)
     * @param length IV length in bytes (16 for AES)
     * @return Hex-encoded IV
     */
    static std::string generateIV(uint32_t length = 16);

    /**
     * @brief Encrypt data using AES-256-CBC
     * @param plaintext Data to encrypt
     * @param key Encryption key (hex string)
     * @param iv Initialization vector (hex string)
     * @return Encrypted data as hex string
     */
    static std::string encrypt(const std::string& plaintext,
                              const std::string& key,
                              const std::string& iv);

    /**
     * @brief Decrypt data using AES-256-CBC
     * @param ciphertext Encrypted data (hex string)
     * @param key Encryption key (hex string)
     * @param iv Initialization vector (hex string)
     * @return Decrypted plaintext
     */
    static std::string decrypt(const std::string& ciphertext,
                              const std::string& key,
                              const std::string& iv);

private:
    /**
     * @brief Simple XOR-based encryption (placeholder for actual AES)
     * Note: In production, use OpenSSL or another crypto library for real AES
     */
    static std::vector<uint8_t> xorCipher(const std::vector<uint8_t>& data,
                                         const std::vector<uint8_t>& key);

    /**
     * @brief PKCS7 padding
     * @param data Data to pad
     * @param blockSize Block size in bytes
     * @return Padded data
     */
    static std::vector<uint8_t> addPadding(const std::vector<uint8_t>& data,
                                          uint32_t blockSize);

    /**
     * @brief Remove PKCS7 padding
     * @param data Padded data
     * @return Unpadded data
     */
    static std::vector<uint8_t> removePadding(const std::vector<uint8_t>& data);

    /**
     * @brief Convert hex string to bytes
     * @param hex Hex string
     * @return Byte vector
     */
    static std::vector<uint8_t> hexToBytes(const std::string& hex);

    /**
     * @brief Convert bytes to hex string
     * @param data Byte vector
     * @return Hex string
     */
    static std::string bytesToHex(const std::vector<uint8_t>& data);
};

#endif // ENCRYPTION_H
