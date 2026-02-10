/*
 * EXEMPLO 3: Sistema de Autenticação com Derivação de Chaves
 * 
 * Use case: Banco de dados que armazena hashes de senhas derivadas
 * 
 * Compilar:
 *   gcc -o auth_system example_auth_system.c agle.c -lssl -lcrypto -O3
 * 
 * Executar:
 *   ./auth_system
 */

#include "agle.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char username[64];
    uint8_t salt[16];              /* Salt para derivação */
    uint8_t password_hash[32];     /* Hash derivado da senha */
    int login_attempts;
    int locked;
} UserAccount;

void bytes_to_hex(const uint8_t *bytes, size_t len, char *hex_out) {
    static const char hex_chars[] = "0123456789abcdef";
    for (size_t i = 0; i < len; i++) {
        hex_out[i * 2] = hex_chars[(bytes[i] >> 4) & 0xF];
        hex_out[i * 2 + 1] = hex_chars[bytes[i] & 0xF];
    }
    hex_out[len * 2] = '\0';
}

int authenticate_user(UserAccount *user, const char *password, AGLE_CTX *ctx) {
    if (user->locked) {
        printf("❌ Conta bloqueada por muitas tentativas de login\n");
        return 0;
    }

    /* Derivar chave usando a mesma senha e salt armazenado */
    uint8_t derived_hash[32];
    AGLE_DeriveKey(
        (uint8_t *)password, strlen(password),
        user->salt, 16,
        100000,  /* 100k iterações */
        derived_hash, 32
    );

    /* Comparar com hash armazenado */
    if (memcmp(derived_hash, user->password_hash, 32) == 0) {
        printf("✅ Autenticação bem-sucedida para %s\n", user->username);
        user->login_attempts = 0;  /* Reset attempts */
        return 1;
    } else {
        user->login_attempts++;
        printf("❌ Senha incorreta. Tentativa %d/3\n", user->login_attempts);

        if (user->login_attempts >= 3) {
            user->locked = 1;
            printf("⚠️  Conta bloqueada após 3 tentativas\n");
        }
        return 0;
    }
}

int main(void) {
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║   Sistema de Autenticação Segura                       ║\n");
    printf("║   Com Derivação de Chaves (KDF)                        ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n\n");

    AGLE_CTX ctx;
    if (!AGLE_Init(&ctx)) {
        printf("❌ Erro ao inicializar AGLE\n");
        return 1;
    }

    /* ========== FASE 1: REGISTRO DE USUÁRIOS ========== */
    printf("FASE 1: Registrando usuários\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");

    UserAccount users[2];
    const char *passwords[2] = {"senha_super_secreta_123!", "outra_senha_forte_abc"};

    for (int i = 0; i < 2; i++) {
        strcpy(users[i].username, (i == 0) ? "alice" : "bob");
        users[i].login_attempts = 0;
        users[i].locked = 0;

        /* 1. Gerar salt aleatório único para cada usuário */
        AGLE_GetRandomBytes(&ctx, users[i].salt, 16);

        /* 2. Derivar hash da senha com salt + iterações */
        AGLE_DeriveKey(
            (uint8_t *)passwords[i], strlen(passwords[i]),
            users[i].salt, 16,
            100000,  /* 100k iterações (recomendado) */
            users[i].password_hash, 32
        );

        /* Mostrar em hex para visualização */
        char hex_salt[33];
        char hex_hash[65];
        bytes_to_hex(users[i].salt, 16, hex_salt);
        bytes_to_hex(users[i].password_hash, 32, hex_hash);

        printf("Usuário: %s\n", users[i].username);
        printf("  Salt: %s\n", hex_salt);
        printf("  Hash: %s\n", hex_hash);
        printf("  Status: ✅ Registrado\n\n");
    }

    /* ========== FASE 2: AUTENTICAÇÃO ========== */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("FASE 2: Testando autenticação\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");

    /* Tentativa 1: Senha correta de Alice */
    printf("Tentativa 1: Alice com senha correta\n");
    authenticate_user(&users[0], "senha_super_secreta_123!", &ctx);
    printf("\n");

    /* Tentativa 2: Senha incorreta de Alice */
    printf("Tentativa 2: Alice com senha incorreta\n");
    authenticate_user(&users[0], "senha_errada", &ctx);
    printf("\n");

    /* Tentativa 3: Bob com senha correta */
    printf("Tentativa 3: Bob com senha correta\n");
    authenticate_user(&users[1], "outra_senha_forte_abc", &ctx);
    printf("\n");

    /* ========== FASE 3: SEGURANÇA ========== */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("FASE 3: Demonstração de segurança\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");

    printf("Boas práticas implementadas:\n");
    printf("✅ 1. Senha nunca é armazenada em texto plano\n");
    printf("✅ 2. Cada usuário tem seu próprio salt único\n");
    printf("✅ 3. Usamos KDF com 100k iterações (resistente a brute force)\n");
    printf("✅ 4. Senhas são derivadas em tempo de login (lento = seguro)\n");
    printf("✅ 5. Limite de 3 tentativas falhas bloqueia a conta\n");
    printf("✅ 6. Memória sensível será limpa com AGLE_SecureZero()\n\n");

    /* Limpeza segura */
    for (int i = 0; i < 2; i++) {
        AGLE_SecureZero(users[i].password_hash, sizeof(users[i].password_hash));
        AGLE_SecureZero(users[i].salt, sizeof(users[i].salt));
    }

    AGLE_Cleanup(&ctx);
    printf("✅ Sistema de autenticação finalizado com segurança\n");

    return 0;
}
