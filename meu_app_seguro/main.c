#include "agle.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    AGLE_CTX ctx;
    
    // Inicializar
    if (!AGLE_Init(&ctx)) {
        fprintf(stderr, "Erro ao inicializar AGLE\n");
        return 1;
    }
    
    // EXEMPLO: Gerar uma senha
    char password[33];
    if (AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, password)) {
        printf("✅ Senha gerada: %s\n", password);
        AGLE_SecureZero(password, sizeof(password));
    }
    
    // EXEMPLO: Gerar um token (32 bytes = 64 chars hex)
    char token[65];
    if (AGLE_GenerateSessionTokenHex(&ctx, token, 32)) {
        printf("✅ Token: %s\n", token);
    }
    
    // EXEMPLO: Hash com SHAKE256 (hexadecimal)
    char hash[65];
    if (AGLE_HashSHAKE256_Hex((uint8_t*)"dados", 5, hash, 32)) {
        printf("✅ Hash: %s\n", hash);
    }
    
    // EXEMPLO: números aleatórios
    uint32_t numero;
    if (AGLE_GetRandomInt(&ctx, 100, &numero)) {
        printf("✅ Número aleatório (0-99): %u\n", numero);
    }
    
    // Limpar
    AGLE_Cleanup(&ctx);
    return 0;
}
