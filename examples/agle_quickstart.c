/*
 * AGLE Library - Quick Start Example
 * Minimal example to get started with AGLE.
 *
 * Compile with:
 *   gcc -Iinclude -o agle_quickstart examples/agle_quickstart.c src/agle.c \
 *       -lssl -lcrypto -O3
 *
 * Run:
 *   ./agle_quickstart
 */

#include "agle.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    printf("AGLE quick start\n\n");

    printf("1. Initializing AGLE context...\n");
    AGLE_CTX ctx;
    if (!AGLE_Init(&ctx)) {
        printf("   Failed to initialize AGLE\n");
        printf("   Make sure /dev/urandom is available\n");
        return 1;
    }
    printf("   AGLE initialized successfully\n\n");

    printf("2. Generating a secure password...\n");
    char password[33];
    AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, password);
    printf("   Password: %s\n\n", password);

    printf("3. Generating a memorable passphrase...\n");
    char passphrase[512];
    AGLE_GeneratePassphrase(&ctx, 6, '-', passphrase);
    printf("   Passphrase: %s\n\n", passphrase);

    printf("4. Generating a session token...\n");
    char api_token[65];
    AGLE_GenerateSessionTokenHex(&ctx, api_token, 32);
    printf("   Token: %s\n\n", api_token);

    printf("5. Creating a hash of a password...\n");
    char hash_hex[65];
    AGLE_HashSHAKE256_Hex(
        (uint8_t*)"mypassword123", 13, hash_hex, 32
    );
    printf("   Hash: %s\n\n", hash_hex);

    printf("6. Deriving encryption key from password...\n");
    const char *master_password = "SecurePassword123!";
    uint8_t salt[16];
    uint8_t derived_key[32];

    AGLE_GetRandomBytes(&ctx, salt, 16);
    AGLE_DeriveKey(
        (uint8_t*)master_password, strlen(master_password),
        salt, 16,
        100000,
        derived_key, 32
    );

    char hex_key[65];
    AGLE_BytesToHex(derived_key, 32, hex_key);
    printf("   Derived Key: %s\n\n", hex_key);

    printf("7. Generating random bytes...\n");
    uint8_t random_bytes[16];
    AGLE_GetRandomBytes(&ctx, random_bytes, 16);

    char hex_random[33];
    AGLE_BytesToHex(random_bytes, 16, hex_random);
    printf("   Random bytes (hex): %s\n\n", hex_random);

    printf("8. Cleaning up...\n");
    AGLE_Cleanup(&ctx);
    printf("   Cleanup complete\n\n");

    printf("All operations completed successfully.\n");
    printf("Next steps:\n");
    printf("- Read LIBRARY.md for API documentation\n");
    printf("- Run: make run (for full examples)\n");
    printf("- Integrate AGLE into your application\n");

    return 0;
}
