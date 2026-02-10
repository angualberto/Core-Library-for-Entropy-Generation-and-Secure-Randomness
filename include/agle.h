/**
 * @file agle.h
 * @brief Public API for AGLE (Alpha-Gauss-Logistic Entropy Generator).
 *
 * Provides C99 APIs for cryptographic RNG, password generation, hashing,
 * key derivation, and session tokens. The implementation relies on OpenSSL
 * for SHAKE256 and system entropy sources for seeding.
 *
 * License: ASL-1.0 (study-only)
 */

#ifndef AGLE_H
#define AGLE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * AGLE Context - Main Structure
 * ============================================================================ */

/**
 * @brief Opaque-like context for AGLE operations.
 */
typedef struct {
    uint8_t state[256];
    size_t position;
    int urandom_fd;
    uint8_t entropy_pool[4096];
} AGLE_CTX;

/* ============================================================================
 * Core RNG Functions
 * ============================================================================ */

/**
 * Initialize AGLE context
 * @param ctx: AGLE context pointer
 * @return: true on success, false on failure
 */
bool AGLE_Init(AGLE_CTX *ctx);

/**
 * Generate cryptographic random bytes
 * @param ctx: AGLE context
 * @param out: Output buffer
 * @param n: Number of bytes to generate
 * @return: true on success, false on failure
 */
bool AGLE_GetRandomBytes(AGLE_CTX *ctx, uint8_t *out, size_t n);

/**
 * Generate a random integer [0, max)
 * @param ctx: AGLE context
 * @param max: Upper bound (exclusive)
 * @param out: Pointer to output value
 * @return: true on success, false on failure
 */
bool AGLE_GetRandomInt(AGLE_CTX *ctx, uint32_t max, uint32_t *out);

/**
 * Generate a random 64-bit integer
 * @param ctx: AGLE context
 * @param out: Pointer to output value
 * @return: true on success, false on failure
 */
bool AGLE_GetRandom64(AGLE_CTX *ctx, uint64_t *out);

/**
 * Cleanup and free resources
 * @param ctx: AGLE context
 */
void AGLE_Cleanup(AGLE_CTX *ctx);

/* ============================================================================
 * Password Generation
 * ============================================================================ */

/**
 * @brief Charset flags for password generation.
 */
typedef enum {
    AGLE_CHARSET_LOWERCASE = 1,    /* a-z */
    AGLE_CHARSET_UPPERCASE = 2,    /* A-Z */
    AGLE_CHARSET_DIGITS = 4,       /* 0-9 */
    AGLE_CHARSET_SYMBOLS = 8,      /* !@#$%^&* */
    AGLE_CHARSET_ALL = 15          /* All of above */
} AGLE_CharsetFlags;

/**
 * Generate a cryptographic password
 * @param ctx: AGLE context
 * @param charset_flags: Character set to use
 * @param length: Password length (min 8, max 1024)
 * @param out: Output buffer (must be length+1 for null terminator)
 * @return: true on success, false on failure
 */
bool AGLE_GeneratePassword(AGLE_CTX *ctx, AGLE_CharsetFlags charset_flags, 
                           size_t length, char *out);

/**
 * Generate a passphrase (word-based password)
 * @param ctx: AGLE context
 * @param num_words: Number of words (3-20)
 * @param separator: Character between words (e.g., '-')
 * @param out: Output buffer (min 512 bytes)
 * @return: true on success, false on failure
 */
bool AGLE_GeneratePassphrase(AGLE_CTX *ctx, size_t num_words, char separator, char *out);

/* ============================================================================
 * Hashing and Digest Functions (SHAKE256)
 * ============================================================================ */

/**
 * Hash data using SHAKE256
 * @param input: Input data
 * @param input_len: Input length
 * @param output: Output buffer
 * @param output_len: Desired output length
 * @return: true on success, false on failure
 */
bool AGLE_HashSHAKE256(const uint8_t *input, size_t input_len,
                       uint8_t *output, size_t output_len);

/**
 * Hash a string using SHAKE256
 * @param str: Null-terminated string
 * @param output: Output buffer
 * @param output_len: Desired output length
 * @return: true on success, false on failure
 */
bool AGLE_HashString(const char *str, uint8_t *output, size_t output_len);

/**
 * Generate SHAKE256 hash in hexadecimal format
 * @param input: Input data
 * @param input_len: Input length
 * @param output: Output hex string buffer (min 2*output_len+1)
 * @param output_len: Desired hash length
 * @return: true on success, false on failure
 */
bool AGLE_HashSHAKE256_Hex(const uint8_t *input, size_t input_len,
                           char *output, size_t output_len);

/* ============================================================================
 * Key Derivation (KDF)
 * ============================================================================ */

/**
 * Key derivation using SHAKE256
 * @param password: Input password
 * @param password_len: Password length
 * @param salt: Salt bytes
 * @param salt_len: Salt length
 * @param iterations: Number of iterations (min 1, recommended 100000)
 * @param key: Output buffer
 * @param key_len: Desired key length
 * @return: true on success, false on failure
 */
bool AGLE_DeriveKey(const uint8_t *password, size_t password_len,
                    const uint8_t *salt, size_t salt_len,
                    uint32_t iterations, uint8_t *key, size_t key_len);

/* ============================================================================
 * Secure Communication Helpers
 * ============================================================================ */

/**
 * Generate a session token for secure communication
 * @param ctx: AGLE context
 * @param token: Output token buffer (min 64 bytes recommended)
 * @param token_len: Token length (16-512)
 * @return: true on success, false on failure
 */
bool AGLE_GenerateSessionToken(AGLE_CTX *ctx, uint8_t *token, size_t token_len);

/**
 * Generate session token in hexadecimal format
 * @param ctx: AGLE context
 * @param token_hex: Output hex string (min 2*token_len+1)
 * @param token_bytes: Desired token byte length
 * @return: true on success, false on failure
 */
bool AGLE_GenerateSessionTokenHex(AGLE_CTX *ctx, char *token_hex, size_t token_bytes);

/**
 * Generate cryptographic nonce (number used once)
 * @param ctx: AGLE context
 * @param nonce: Output buffer (min 16 bytes)
 * @return: 64-bit nonce value
 */
uint64_t AGLE_GenerateNonce(AGLE_CTX *ctx, uint8_t *nonce);

/* ============================================================================
 * Utility Functions
 * ============================================================================ */

/**
 * Convert bytes to hexadecimal string
 * @param bytes: Input bytes
 * @param len: Number of bytes
 * @param hex_out: Output hex string (min 2*len+1)
 * @return: Pointer to hex string (same as hex_out)
 */
char* AGLE_BytesToHex(const uint8_t *bytes, size_t len, char *hex_out);

/**
 * Convert hexadecimal string to bytes
 * @param hex_in: Input hex string
 * @param bytes_out: Output bytes
 * @param max_len: Maximum output length
 * @return: Number of bytes written, or -1 on error
 */
int AGLE_HexToBytes(const char *hex_in, uint8_t *bytes_out, size_t max_len);

/**
 * Securely clear sensitive data from memory
 * @param data: Data to clear
 * @param len: Length of data
 */
void AGLE_SecureZero(void *data, size_t len);

/* ============================================================================
 * Version and Info
 * ============================================================================ */

#define AGLE_VERSION "2.0.0"
#define AGLE_VERSION_MAJOR 2
#define AGLE_VERSION_MINOR 0
#define AGLE_VERSION_PATCH 0

/**
 * Get AGLE version string
 * @return: Version string
 */
const char* AGLE_GetVersion(void);

/**
 * Get library info and features
 * @return: Info string
 */
const char* AGLE_GetInfo(void);

#ifdef __cplusplus
}
#endif

#endif /* AGLE_H */
