#include "agle.h"
#include <stdio.h>

int main(void) {
    AGLE_CTX ctx;
    
    printf("╔═══════════════════════════════════════════╗\n");
    printf("║   EXEMPLO SIMPLES - BIBLIOTECA AGLE      ║\n");
    printf("╚═══════════════════════════════════════════╝\n\n");
    
    // 1. INICIALIZAR (sempre primeiro!)
    if (!AGLE_Init(&ctx)) {
        printf("❌ Erro ao inicializar!\n");
        return 1;
    }
    printf("✅ AGLE inicializada!\n\n");
    
    // 2. GERAR SENHA
    printf("📝 GERAR SENHA:\n");
    char senha[33];
    AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, senha);
    printf("   Senha: %s\n\n", senha);
    
    // 3. GERAR NÚMERO ALEATÓRIO
    printf("🎲 NÚMERO ALEATÓRIO:\n");
    uint32_t numero;
    AGLE_GetRandomInt(&ctx, 100, &numero);
    printf("   Número (0-99): %u\n\n", numero);
    
    // 4. GERAR TOKEN
    printf("🎫 TOKEN DE SESSÃO:\n");
    char token[65];
    AGLE_GenerateSessionTokenHex(&ctx, token, 32);
    printf("   Token: %s\n\n", token);
    
    // 5. FAZER HASH
    printf("#️⃣  HASH (SHAKE256):\n");
    char hash[65];
    AGLE_HashSHAKE256_Hex((uint8_t*)"minha senha", 11, hash, 32);
    printf("   Hash: %s\n\n", hash);
    
    // 6. GERAR PASSPHRASE
    printf("🔤 PASSPHRASE (memorável):\n");
    char frase[256];
    AGLE_GeneratePassphrase(&ctx, 5, '-', frase);
    printf("   Frase: %s\n\n", frase);
    
    // 7. LIMPAR (sempre no final!)
    AGLE_Cleanup(&ctx);
    printf("✅ AGLE finalizada!\n");
    
    printf("\n╔═══════════════════════════════════════════╗\n");
    printf("║        PRONTO! Simples assim! 🎉         ║\n");
    printf("╚═══════════════════════════════════════════╝\n");
    
    return 0;
}
