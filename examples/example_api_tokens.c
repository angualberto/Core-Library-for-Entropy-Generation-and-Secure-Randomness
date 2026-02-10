/*
 * EXEMPLO 2: Aplicação de Geração de API Tokens
 * 
 * Use case: API REST que precisa gerar tokens de autenticação
 * 
 * Compilar:
 *   gcc -o api_tokengen example_api_tokens.c agle.c -lssl -lcrypto -O3
 * 
 * Executar:
 *   ./api_tokengen
 */

#include "agle.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct {
    char client_id[64];
    char api_token[65];   /* 64 hex chars + '\0' */
    char api_secret[65];  /* 64 hex chars + '\0' */
    time_t created_at;
    int active;
} APIKey;

int main(void) {
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║   Sistema de Geração de API Keys                       ║\n");
    printf("║   Usando AGLE Cryptographic Library                   ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n\n");

    AGLE_CTX ctx;
    if (!AGLE_Init(&ctx)) {
        printf("❌ Erro ao inicializar AGLE\n");
        return 1;
    }

    /* Criar 3 clientes API */
    APIKey clients[3];
    const char *client_names[] = {
        "mobile_app_v2",
        "web_dashboard",
        "IoT_device_farm"
    };

    printf("Gerando chaves API para clientes:\n\n");

    for (int i = 0; i < 3; i++) {
        /* Client ID */
        strcpy(clients[i].client_id, client_names[i]);
        clients[i].created_at = time(NULL);
        clients[i].active = 1;

        /* Gerar API Token (32 bytes = 64 hex chars) */
        AGLE_GenerateSessionTokenHex(&ctx, clients[i].api_token, 32);

        /* Gerar API Secret (32 bytes = 64 hex chars) */
        AGLE_GenerateSessionTokenHex(&ctx, clients[i].api_secret, 32);

        printf("Client %d: %s\n", i + 1, clients[i].client_id);
        printf("  Token:  %s\n", clients[i].api_token);
        printf("  Secret: %s\n", clients[i].api_secret);
        printf("\n");
    }

    printf("═══════════════════════════════════════════════════════════\n\n");

    /* Simular verificação de token */
    printf("Verificando tokens:\n\n");
    
    const char *test_token = clients[0].api_token;
    printf("Token fornecido: %s\n", test_token);
    printf("Token esperado:  %s\n", clients[0].api_token);
    
    if (strcmp(test_token, clients[0].api_token) == 0) {
        printf("✅ Token válido! Autenticação aceita.\n\n");
    } else {
        printf("❌ Token inválido! Acesso negado.\n\n");
    }

    /* Gerar rate limiting tokens (ephemeral) */
    printf("═══════════════════════════════════════════════════════════\n");
    printf("Gerando tokens efêmeros para rate limiting:\n\n");

    for (int i = 0; i < 3; i++) {
        uint64_t request_id;
        AGLE_GetRandom64(&ctx, &request_id);
        
        char nonce[17];
        uint8_t nonce_bytes[8];
        AGLE_GenerateNonce(&ctx, nonce_bytes);
        AGLE_BytesToHex(nonce_bytes, 8, nonce);

        printf("Request #%d from %s\n", i + 1, clients[0].client_id);
        printf("  Request ID: %lu\n", request_id);
        printf("  Nonce:      %s\n", nonce);
        printf("\n");
    }

    AGLE_Cleanup(&ctx);
    printf("✅ API Token generation complete!\n");

    return 0;
}
