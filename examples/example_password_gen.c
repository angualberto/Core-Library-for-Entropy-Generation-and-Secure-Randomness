/*
 * EXEMPLO 1: Aplicação de Geração de Senhas
 * 
 * Use case: Sistema de cadastro que precisa gerar senhas temporárias
 * 
 * Compilar:
 *   gcc -o password_gen password_gen.c agle.c -lssl -lcrypto -O3
 * 
 * Executar:
 *   ./password_gen
 */

#include "agle.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct {
    char username[64];
    char temp_password[33];
    time_t created_at;
} UserAccount;

int main(void) {
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║   Sistema de Geração de Senhas Temporárias             ║\n");
    printf("║   Usando AGLE Cryptographic Library                   ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n\n");

    /* Inicializar AGLE */
    AGLE_CTX ctx;
    if (!AGLE_Init(&ctx)) {
        printf("❌ Erro ao inicializar AGLE\n");
        return 1;
    }

    /* Simular cadastro de 5 usuários */
    UserAccount users[5] = {
        {"alice@example.com", "", 0},
        {"bob@example.com", "", 0},
        {"carlos@example.com", "", 0},
        {"diana@example.com", "", 0},
        {"eva@example.com", "", 0}
    };

    printf("Gerando senhas temporárias:\n\n");
    printf("%-25s %-35s\n", "Usuário", "Senha Temporária");
    printf("%s\n", "═══════════════════════════════════════════════════════════");

    for (int i = 0; i < 5; i++) {
        /* Gerar senha forte de 32 caracteres */
        AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, users[i].temp_password);
        users[i].created_at = time(NULL);

        printf("%-25s %s\n", users[i].username, users[i].temp_password);
    }

    printf("\n");
    printf("Senhas geradas com sucesso! ✅\n");
    printf("Recomendação: Enviar via email seguro ou SMS.\n\n");

    /* Hash da senha para armazenar no banco de dados */
    printf("═══════════════════════════════════════════════════════════\n");
    printf("Hashes das senhas para banco de dados:\n\n");

    for (int i = 0; i < 5; i++) {
        char hash_hex[65];
        AGLE_HashString(users[i].temp_password, (uint8_t *)hash_hex, 32);

        /* Converter para hexadecimal real */
        uint8_t hash[32];
        AGLE_HashSHAKE256((uint8_t *)users[i].temp_password, 
                         strlen(users[i].temp_password),
                         hash, 32);
        AGLE_BytesToHex(hash, 32, hash_hex);

        printf("User: %-20s | Hash: %s\n", users[i].username, hash_hex);
    }

    /* Limpeza de senhas em memória */
    for (int i = 0; i < 5; i++) {
        AGLE_SecureZero(users[i].temp_password, sizeof(users[i].temp_password));
    }

    AGLE_Cleanup(&ctx);
    printf("\n✅ Senhas limpas da memória com segurança\n");

    return 0;
}
