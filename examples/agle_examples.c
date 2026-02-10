/*
 * AGLE Library - Examples of Usage
 * Demonstrates RNG, password generation, hashing, and secure communication
 */

#include "agle.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

/* ============================================================================
 * Example 1: Generate Cryptographic Random Numbers
 * ============================================================================ */
void example_rng(AGLE_CTX *ctx) {
    printf("\n=== Example 1: Random Number Generation ===\n");

    /* Generate 32 random bytes */
    uint8_t random_bytes[32];
    if (!AGLE_GetRandomBytes(ctx, random_bytes, sizeof(random_bytes))) {
        printf("Error generating random bytes\n");
        return;
    }

    printf("32 random bytes (hex): ");
    char hex_buf[65];
    AGLE_BytesToHex(random_bytes, 32, hex_buf);
    printf("%s\n", hex_buf);

    /* Generate random integers */
    uint32_t random_int;
    if (!AGLE_GetRandomInt(ctx, 100, &random_int)) {
        printf("Error generating random int\n");
        return;
    }
    printf("Random number [0-99]: %u\n", random_int);

    /* Generate 64-bit random number */
    uint64_t random_64;
    if (!AGLE_GetRandom64(ctx, &random_64)) {
        printf("Error generating 64-bit random\n");
        return;
    }
    printf("Random 64-bit: %lu\n", random_64);
}

/* ============================================================================
 * Example 2: Secure Password Generation
 * ============================================================================ */
void example_passwords(AGLE_CTX *ctx) {
    printf("\n=== Example 2: Secure Password Generation ===\n");

    char password[33];

    /* Strong password: letters + digits + symbols, 32 chars */
    if (!AGLE_GeneratePassword(ctx, AGLE_CHARSET_ALL, 32, password)) {
        printf("Error generating password\n");
        return;
    }
    printf("Strong password (32 chars): %s\n", password);

    /* Medium password: letters + digits only, 24 chars */
    if (!AGLE_GeneratePassword(ctx, AGLE_CHARSET_UPPERCASE | AGLE_CHARSET_LOWERCASE | AGLE_CHARSET_DIGITS, 
                              24, password)) {
        printf("Error generating password\n");
        return;
    }
    printf("Medium password (24 chars): %s\n", password);

    /* Alphanumeric password, 16 chars */
    if (!AGLE_GeneratePassword(ctx, AGLE_CHARSET_LOWERCASE | AGLE_CHARSET_DIGITS, 16, password)) {
        printf("Error generating password\n");
        return;
    }
    printf("Alphanumeric (16 chars):    %s\n", password);
}

/* ============================================================================
 * Example 3: Passphrases (memorable passwords)
 * ============================================================================ */
void example_passphrases(AGLE_CTX *ctx) {
    printf("\n=== Example 3: Passphrase Generation ===\n");

    char passphrase[512];

    /* Generate 5-word passphrase with hyphens */
    if (!AGLE_GeneratePassphrase(ctx, 5, '-', passphrase)) {
        printf("Error generating passphrase\n");
        return;
    }
    printf("5-word passphrase:  %s\n", passphrase);

    /* Generate 8-word passphrase with spaces */
    if (!AGLE_GeneratePassphrase(ctx, 8, ' ', passphrase)) {
        printf("Error generating passphrase\n");
        return;
    }
    printf("8-word passphrase:  %s\n", passphrase);
}

/* ============================================================================
 * Example 4: Cryptographic Hashing (SHAKE256)
 * ============================================================================ */
void example_hashing(AGLE_CTX *ctx) {
    printf("\n=== Example 4: Cryptographic Hashing (SHAKE256) ===\n");
    (void)ctx; /* Not used in this example */

    const char *text = "Hello, Cryptographic World!";
    
    /* Hash to 32 bytes (256 bits) */
    uint8_t hash256[32];
    if (!AGLE_HashString(text, hash256, sizeof(hash256))) {
        printf("Error hashing\n");
        return;
    }
    
    char hex_hash[65];
    AGLE_BytesToHex(hash256, sizeof(hash256), hex_hash);
    printf("Text: %s\n", text);
    printf("Hash (256-bit): %s\n", hex_hash);

    /* Hash to 64 bytes (512 bits) */
    uint8_t hash512[64];
    if (!AGLE_HashString(text, hash512, sizeof(hash512))) {
        printf("Error hashing\n");
        return;
    }
    
    char hex_hash512[129];
    AGLE_BytesToHex(hash512, sizeof(hash512), hex_hash512);
    printf("Hash (512-bit): %s\n", hex_hash512);

    /* Direct hex output */
    char hex_direct[65];
    if (!AGLE_HashSHAKE256_Hex((const uint8_t *)text, strlen(text), hex_direct, 32)) {
        printf("Error hashing\n");
        return;
    }
    printf("Direct hex out:  %s\n", hex_direct);
}

/* ============================================================================
 * Example 5: Key Derivation Function (KDF)
 * ============================================================================ */
void example_key_derivation(AGLE_CTX *ctx) {
    printf("\n=== Example 5: Key Derivation (KDF) ===\n");
    (void)ctx; /* Not used directly */

    const char *password = "MySecurePassword123!";
    uint8_t salt[16];
    
    /* Generate random salt */
    AGLE_CTX *init_ctx = &(AGLE_CTX){0};
    if (!AGLE_Init(init_ctx)) {
        printf("Error initializing AGLE\n");
        return;
    }
    
    if (!AGLE_GetRandomBytes(init_ctx, salt, sizeof(salt))) {
        printf("Error generating salt\n");
        AGLE_Cleanup(init_ctx);
        return;
    }

    /* Derive a 32-byte key with 100,000 iterations */
    uint8_t derived_key[32];
    if (!AGLE_DeriveKey((const uint8_t *)password, strlen(password),
                        salt, sizeof(salt), 100000, derived_key, sizeof(derived_key))) {
        printf("Error deriving key\n");
        AGLE_Cleanup(init_ctx);
        return;
    }

    char hex_salt[33];
    char hex_key[65];
    AGLE_BytesToHex(salt, sizeof(salt), hex_salt);
    AGLE_BytesToHex(derived_key, sizeof(derived_key), hex_key);

    printf("Password:        %s\n", password);
    printf("Salt (16 bytes):     %s\n", hex_salt);
    printf("Derived key (32 bytes): %s\n", hex_key);

    AGLE_Cleanup(init_ctx);
}

/* ============================================================================
 * Example 6: Session Tokens (for secure communication)
 * ============================================================================ */
void example_session_tokens(AGLE_CTX *ctx) {
    printf("\n=== Example 6: Session Tokens ===\n");

    /* Generate 32-byte session token */
    uint8_t token[32];
    if (!AGLE_GenerateSessionToken(ctx, token, sizeof(token))) {
        printf("Error generating session token\n");
        return;
    }

    char hex_token[65];
    AGLE_BytesToHex(token, sizeof(token), hex_token);
    printf("Session token (hex): %s\n", hex_token);

    /* Generate hex token directly */
    char hex_token_direct[65];
    if (!AGLE_GenerateSessionTokenHex(ctx, hex_token_direct, 32)) {
        printf("Error generating hex token\n");
        return;
    }
    printf("Direct hex token:    %s\n", hex_token_direct);

    /* Generate nonce */
    uint8_t nonce[8];
    uint64_t nonce_val = AGLE_GenerateNonce(ctx, nonce);
    printf("Nonce (64-bit):      %lu\n", nonce_val);
}

/* ============================================================================
 * Example 7: Practical Use Case - API Key Generation
 * ============================================================================ */
void example_api_key_generation(AGLE_CTX *ctx) {
    printf("\n=== Example 7: API Key Generation ===\n");

    /* Generate a 64-character API key (48 random bytes in base16) */
    uint8_t api_key[48];
    if (!AGLE_GetRandomBytes(ctx, api_key, sizeof(api_key))) {
        printf("Error generating API key\n");
        return;
    }

    char api_key_hex[97];
    AGLE_BytesToHex(api_key, sizeof(api_key), api_key_hex);
    printf("API Key: %s\n", api_key_hex);
}

/* ============================================================================
 * Example 8: Multiple Passwords with Different Requirements
 * ============================================================================ */
void example_password_suite(AGLE_CTX *ctx) {
    printf("\n=== Example 8: Password Suite ===\n");

    char pwd[65];

    /* Admin password: max security */
    if (!AGLE_GeneratePassword(ctx, AGLE_CHARSET_ALL, 32, pwd)) {
        printf("Error generating password\n");
        return;
    }
    printf("Admin password:  %s\n", pwd);

    /* User password: strong but shorter */
    if (!AGLE_GeneratePassword(ctx, AGLE_CHARSET_ALL, 24, pwd)) {
        printf("Error generating password\n");
        return;
    }
    printf("User password:   %s\n", pwd);

    /* PIN: digits only, 8 digits */
    if (!AGLE_GeneratePassword(ctx, AGLE_CHARSET_DIGITS, 8, pwd)) {
        printf("Error generating password\n");
        return;
    }
    printf("PIN (8 digits):  %s\n", pwd);
}

/* ============================================================================
 * Main Function
 * ============================================================================ */
int main(void) {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║          AGLE Library Examples                             ║\n");
    printf("║   Cryptographic RNG & Secure Communication                 ║\n");
    printf("║                                                            ║\n");
    printf("║   %s\n", AGLE_GetVersion());
    printf("╚════════════════════════════════════════════════════════════╝\n");

    /* Initialize AGLE context */
    AGLE_CTX ctx;
    if (!AGLE_Init(&ctx)) {
        printf("Error: Failed to initialize AGLE context\n");
        printf("Make sure /dev/urandom is available\n");
        return 1;
    }

    printf("\n✓ AGLE initialized successfully\n");

    /* Run all examples */
    example_rng(&ctx);
    example_passwords(&ctx);
    example_passphrases(&ctx);
    example_hashing(&ctx);
    example_key_derivation(&ctx);
    example_session_tokens(&ctx);
    example_api_key_generation(&ctx);
    example_password_suite(&ctx);

    /* Print library info */
    printf("\n╔════════════════════════════════════════════════════════════╗");
    printf("\n%s\n", AGLE_GetInfo());
    printf("╚════════════════════════════════════════════════════════════╝\n");

    /* Cleanup */
    AGLE_Cleanup(&ctx);
    printf("\n✓ AGLE cleanup complete\n");

    return 0;
}
