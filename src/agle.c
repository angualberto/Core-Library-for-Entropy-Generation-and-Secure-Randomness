/**
 * @file agle.c
 * @brief AGLE implementation.
 */

#include "agle.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define URANDOM_PATH "/dev/urandom"
#define RAW_ENTROPY_CHUNK 4096

/* ============================================================================
 * Internal Helper Functions
 * ============================================================================ */

static const char *CHARSET_LOWER = "abcdefghijklmnopqrstuvwxyz";
static const char *CHARSET_UPPER = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char *CHARSET_DIGITS = "0123456789";
static const char *CHARSET_SYMBOLS = "!@#$%^&*-_+=[]{}()|:;<>?,./";

static const char *WORDLIST[] = {
    "alpha", "beta", "gamma", "delta", "epsilon", "zeta", "eta", "theta",
    "iota", "kappa", "lambda", "mu", "nu", "xi", "omicron", "pi",
    "rho", "sigma", "tau", "upsilon", "phi", "chi", "psi", "omega",
    "red", "blue", "green", "yellow", "purple", "orange", "black", "white",
    "cat", "dog", "bird", "fish", "lion", "tiger", "bear", "eagle",
    "mountain", "river", "ocean", "forest", "desert", "storm", "fire", "water",
    "sun", "moon", "star", "planet", "galaxy", "universe", "space", "light",
    "crypto", "secure", "random", "cipher", "hash", "key", "chain", "block"
};

#define WORDLIST_SIZE (sizeof(WORDLIST) / sizeof(WORDLIST[0]))

static bool _read_urandom(uint8_t *buf, size_t n) {
    int fd = open(URANDOM_PATH, O_RDONLY);
    if (fd < 0) {
        perror("open /dev/urandom");
        return false;
    }

    ssize_t rd = read(fd, buf, n);
    close(fd);
    return (rd == (ssize_t)n);
}

/* ============================================================================
 * Core RNG Functions
 * ============================================================================ */

bool AGLE_Init(AGLE_CTX *ctx) {
    if (ctx == NULL) return false;

    memset(ctx, 0, sizeof(AGLE_CTX));
    
    if (!_read_urandom(ctx->state, sizeof(ctx->state))) {
        return false;
    }
    
    if (!_read_urandom(ctx->entropy_pool, sizeof(ctx->entropy_pool))) {
        return false;
    }

    ctx->position = 0;
    return true;
}

bool AGLE_GetRandomBytes(AGLE_CTX *ctx, uint8_t *out, size_t n) {
    if (ctx == NULL || out == NULL || n == 0) return false;

    uint8_t raw_buf[RAW_ENTROPY_CHUNK];
    size_t produced = 0;

    while (produced < n) {
        if (!_read_urandom(raw_buf, RAW_ENTROPY_CHUNK)) {
            return false;
        }

        EVP_MD_CTX *mctx = EVP_MD_CTX_new();
        if (mctx == NULL) return false;

        EVP_DigestInit(mctx, EVP_shake256());
        EVP_DigestUpdate(mctx, raw_buf, RAW_ENTROPY_CHUNK);
        EVP_DigestUpdate(mctx, ctx->state, sizeof(ctx->state));

        size_t to_squeeze = (n - produced) > RAW_ENTROPY_CHUNK ? 
                            RAW_ENTROPY_CHUNK : (n - produced);
        
        EVP_DigestFinalXOF(mctx, out + produced, to_squeeze);
        EVP_MD_CTX_free(mctx);

        produced += to_squeeze;
    }

    AGLE_SecureZero(raw_buf, sizeof(raw_buf));
    return true;
}

bool AGLE_GetRandomInt(AGLE_CTX *ctx, uint32_t max, uint32_t *out) {
    if (ctx == NULL || max == 0 || out == NULL) return false;

    uint32_t random_value;
    if (!AGLE_GetRandomBytes(ctx, (uint8_t *)&random_value, sizeof(random_value))) {
        return false;
    }

    *out = random_value % max;
    return true;
}

bool AGLE_GetRandom64(AGLE_CTX *ctx, uint64_t *out) {
    if (ctx == NULL || out == NULL) return false;
    return AGLE_GetRandomBytes(ctx, (uint8_t *)out, sizeof(uint64_t));
}

void AGLE_Cleanup(AGLE_CTX *ctx) {
    if (ctx == NULL) return;
    AGLE_SecureZero(ctx, sizeof(AGLE_CTX));
}

/* ============================================================================
 * Password Generation
 * ============================================================================ */

bool AGLE_GeneratePassword(AGLE_CTX *ctx, AGLE_CharsetFlags charset_flags,
                           size_t length, char *out) {
    if (ctx == NULL || out == NULL || length < 8 || length > 1024) {
        return false;
    }

    char charset[256] = {0};
    size_t charset_len = 0;

    if (charset_flags & AGLE_CHARSET_LOWERCASE) {
        strcat(charset, CHARSET_LOWER);
    }
    if (charset_flags & AGLE_CHARSET_UPPERCASE) {
        strcat(charset, CHARSET_UPPER);
    }
    if (charset_flags & AGLE_CHARSET_DIGITS) {
        strcat(charset, CHARSET_DIGITS);
    }
    if (charset_flags & AGLE_CHARSET_SYMBOLS) {
        strcat(charset, CHARSET_SYMBOLS);
    }

    charset_len = strlen(charset);
    if (charset_len == 0) return false;

    for (size_t i = 0; i < length; i++) {
        uint32_t idx;
        if (!AGLE_GetRandomInt(ctx, (uint32_t)charset_len, &idx)) {
            return false;
        }
        out[i] = charset[idx];
    }
    out[length] = '\0';

    AGLE_SecureZero(charset, sizeof(charset));
    return true;
}

bool AGLE_GeneratePassphrase(AGLE_CTX *ctx, size_t num_words, 
                            char separator, char *out) {
    if (ctx == NULL || out == NULL || num_words < 3 || num_words > 20) {
        return false;
    }

    out[0] = '\0';

    for (size_t i = 0; i < num_words; i++) {
        uint32_t word_idx;
        if (!AGLE_GetRandomInt(ctx, (uint32_t)WORDLIST_SIZE, &word_idx)) {
            return false;
        }

        if (i > 0) {
            char sep[2] = {separator, '\0'};
            strcat(out, sep);
        }
        strcat(out, WORDLIST[word_idx]);
    }

    return true;
}

/* ============================================================================
 * Hashing and Digest Functions
 * ============================================================================ */

bool AGLE_HashSHAKE256(const uint8_t *input, size_t input_len,
                       uint8_t *output, size_t output_len) {
    if (input == NULL || output == NULL || output_len == 0) return false;

    EVP_MD_CTX *mctx = EVP_MD_CTX_new();
    if (mctx == NULL) return false;

    bool result = true;
    if (!EVP_DigestInit(mctx, EVP_shake256())) {
        result = false;
        goto cleanup;
    }
    
    if (!EVP_DigestUpdate(mctx, input, input_len)) {
        result = false;
        goto cleanup;
    }
    
    if (!EVP_DigestFinalXOF(mctx, output, output_len)) {
        result = false;
        goto cleanup;
    }

cleanup:
    EVP_MD_CTX_free(mctx);
    return result;
}

bool AGLE_HashString(const char *str, uint8_t *output, size_t output_len) {
    if (str == NULL || output == NULL || output_len == 0) return false;
    return AGLE_HashSHAKE256((const uint8_t *)str, strlen(str), output, output_len);
}

bool AGLE_HashSHAKE256_Hex(const uint8_t *input, size_t input_len,
                           char *output, size_t output_len) {
    if (input == NULL || output == NULL || output_len == 0) return false;

    uint8_t hash[output_len];
    if (!AGLE_HashSHAKE256(input, input_len, hash, output_len)) {
        return false;
    }

    return AGLE_BytesToHex(hash, output_len, output) != NULL;
}

/* ============================================================================
 * Key Derivation
 * ============================================================================ */

bool AGLE_DeriveKey(const uint8_t *password, size_t password_len,
                    const uint8_t *salt, size_t salt_len,
                    uint32_t iterations, uint8_t *key, size_t key_len) {
    if (password == NULL || salt == NULL || key == NULL) return false;
    if (iterations < 1) return false;

    uint8_t temp[key_len];
    memset(temp, 0, key_len);
    memcpy(temp, password, password_len < key_len ? password_len : key_len);

    for (uint32_t i = 0; i < iterations; i++) {
        EVP_MD_CTX *mctx = EVP_MD_CTX_new();
        if (mctx == NULL) return false;

        EVP_DigestInit(mctx, EVP_shake256());
        EVP_DigestUpdate(mctx, temp, key_len);
        EVP_DigestUpdate(mctx, salt, salt_len);
        EVP_DigestFinalXOF(mctx, temp, key_len);
        EVP_MD_CTX_free(mctx);
    }

    memcpy(key, temp, key_len);
    AGLE_SecureZero(temp, key_len);
    return true;
}

/* ============================================================================
 * Secure Communication Helpers
 * ============================================================================ */

bool AGLE_GenerateSessionToken(AGLE_CTX *ctx, uint8_t *token, size_t token_len) {
    if (ctx == NULL || token == NULL || token_len < 16 || token_len > 512) {
        return false;
    }
    return AGLE_GetRandomBytes(ctx, token, token_len);
}

bool AGLE_GenerateSessionTokenHex(AGLE_CTX *ctx, char *token_hex, size_t token_bytes) {
    if (ctx == NULL || token_hex == NULL || token_bytes == 0) return false;

    uint8_t token[token_bytes];
    if (!AGLE_GenerateSessionToken(ctx, token, token_bytes)) {
        return false;
    }

    char *result = AGLE_BytesToHex(token, token_bytes, token_hex);
    AGLE_SecureZero(token, token_bytes);
    return result != NULL;
}

uint64_t AGLE_GenerateNonce(AGLE_CTX *ctx, uint8_t *nonce) {
    if (ctx == NULL || nonce == NULL) return 0;

    uint64_t nonce_val;
    if (!AGLE_GetRandom64(ctx, &nonce_val)) {
        return 0;
    }

    if (nonce != NULL) {
        memcpy(nonce, &nonce_val, sizeof(nonce_val));
    }

    return nonce_val;
}

/* ============================================================================
 * Utility Functions
 * ============================================================================ */

char* AGLE_BytesToHex(const uint8_t *bytes, size_t len, char *hex_out) {
    if (bytes == NULL || hex_out == NULL || len == 0) return NULL;

    static const char hex_chars[] = "0123456789abcdef";
    for (size_t i = 0; i < len; i++) {
        hex_out[i * 2] = hex_chars[(bytes[i] >> 4) & 0xF];
        hex_out[i * 2 + 1] = hex_chars[bytes[i] & 0xF];
    }
    hex_out[len * 2] = '\0';

    return hex_out;
}

int AGLE_HexToBytes(const char *hex_in, uint8_t *bytes_out, size_t max_len) {
    if (hex_in == NULL || bytes_out == NULL) return -1;

    size_t hex_len = strlen(hex_in);
    if (hex_len % 2 != 0 || hex_len / 2 > max_len) return -1;

    for (size_t i = 0; i < hex_len; i += 2) {
        char byte_str[3] = {hex_in[i], hex_in[i + 1], '\0'};
        char *endptr;
        long value = strtol(byte_str, &endptr, 16);
        
        if (*endptr != '\0' || value < 0 || value > 255) {
            return -1;
        }

        bytes_out[i / 2] = (uint8_t)value;
    }

    return (int)(hex_len / 2);
}

void AGLE_SecureZero(void *data, size_t len) {
    if (data == NULL || len == 0) return;
    memset(data, 0, len);
    __asm__ __volatile__("" : : "r"(data) : "memory");
}

/* ============================================================================
 * Version and Info
 * ============================================================================ */

const char* AGLE_GetVersion(void) {
    return AGLE_VERSION;
}

const char* AGLE_GetInfo(void) {
    return "AGLE v" AGLE_VERSION " - Alpha-Gauss-Logistic Entropy Generator\n"
           "Features: RNG, Password Generation, SHAKE256 Hashing, KDF, Session Tokens\n"
           "License: ASL-1.0 (study-only)";
}
