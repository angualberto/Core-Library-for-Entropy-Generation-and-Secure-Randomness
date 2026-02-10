#include "agle.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/**
 * PASSWORD VALIDATOR - Programa para validar senhas com AGLE
 * Funciona como "ponte" entre HTML/Web e criptografia C
 */

#define MAX_PASSWORD_LEN 256
#define MAX_USERS 100

typedef struct {
    char username[64];
    uint8_t salt[16];
    uint8_t hash[32];
    int login_attempts;
    time_t locked_until;
} UserRecord;

UserRecord users[MAX_USERS];
int user_count = 0;

/**
 * Mascara uma senha para exibição (mostra só * e primeiros/últimos chars)
 */
void mascarar_senha(const char *senha, char *masked) {
    int len = strlen(senha);
    if (len <= 2) {
        strcpy(masked, "***");
        return;
    }
    
    sprintf(masked, "%c", senha[0]);
    for (int i = 1; i < len - 1; i++) {
        strcat(masked, "*");
    }
    sprintf(masked + strlen(masked), "%c", senha[len - 1]);
}

/**
 * Registrar novo usuário com senha
 */
bool registrar_usuario(AGLE_CTX *ctx, const char *username, const char *password) {
    if (user_count >= MAX_USERS) {
        printf("❌ Banco de usuários cheio!\n");
        return false;
    }

    // Verificar se usuário existe
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("❌ Usuário '%s' já existe!\n", username);
            return false;
        }
    }

    UserRecord *user = &users[user_count];
    strcpy(user->username, username);
    user->login_attempts = 0;
    user->locked_until = 0;

    // 1. Gerar salt aleatório
    if (!AGLE_GetRandomBytes(ctx, user->salt, 16)) {
        printf("❌ Erro ao gerar salt!\n");
        return false;
    }

    // 2. Derivar hash com KDF
    if (!AGLE_DeriveKey((uint8_t*)password, strlen(password),
                        user->salt, 16, 100000, user->hash, 32)) {
        printf("❌ Erro ao derivar chave!\n");
        return false;
    }

    user_count++;

    // Exibir info (mascará a senha)
    char masked[MAX_PASSWORD_LEN];
    mascarar_senha(password, masked);
    
    printf("\n✅ Usuário Registrado!\n");
    printf("├─ Usuário: %s\n", username);
    printf("├─ Senha: %s (mascarada para exibição)\n", masked);
    printf("└─ Salt armazenado: ");
    for (int i = 0; i < 16; i++) printf("%02x", user->salt[i]);
    printf("\n");

    return true;
}

/**
 * Autenticar usuário (validar senha)
 */
bool autenticar_usuario(AGLE_CTX *ctx, const char *username, const char *password) {
    (void)ctx;  // Parâmetro mantido para consistência de API
    
    // Procurar usuário
    UserRecord *user = NULL;
    
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            user = &users[i];
            break;
        }
    }

    if (user == NULL) {
        printf("❌ Usuário '%s' não encontrado!\n", username);
        return false;
    }

    // Verificar se está bloqueado
    if (user->locked_until > time(NULL)) {
        printf("❌ Conta bloqueada! Tente novamente mais tarde.\n");
        return false;
    }

    // Derivar hash com o salt armazenado
    uint8_t hash_tentativa[32];
    if (!AGLE_DeriveKey((uint8_t*)password, strlen(password),
                        user->salt, 16, 100000, hash_tentativa, 32)) {
        printf("❌ Erro ao processar senha!\n");
        return false;
    }

    // Comparar hashes
    if (memcmp(user->hash, hash_tentativa, 32) == 0) {
        // Sucesso!
        user->login_attempts = 0;
        user->locked_until = 0;

        printf("\n✅ AUTENTICAÇÃO SUCESSO!\n");
        printf("├─ Benvindo, %s!\n", username);
        printf("├─ Tentativas (resetadas): 0/3\n");
        printf("└─ Status: Desbloqueado\n");
        return true;
    } else {
        // Falha!
        user->login_attempts++;
        
        if (user->login_attempts >= 3) {
            user->locked_until = time(NULL) + 300; // 5 minutos
            printf("❌ SENHA INCORRETA!\n");
            printf("├─ Tentativas: %d/3\n", user->login_attempts);
            printf("└─ ⚠️ Conta BLOQUEADA por 5 minutos!\n");
            return false;
        } else {
            printf("❌ SENHA INCORRETA!\n");
            printf("├─ Tentativas: %d/3\n", user->login_attempts);
            printf("└─ Cuidado! Mais 2 tentativas e conta bloqueia.\n");
            return false;
        }
    }
}

/**
 * Listar todos os usuários (sem mostrar hashes!)
 */
void listar_usuarios() {
    if (user_count == 0) {
        printf("ℹ️  Nenhum usuário registrado.\n");
        return;
    }

    printf("\n📋 USUÁRIOS REGISTRADOS:\n");
    printf("┌──────────────────┬──────────────────┬──────────────┐\n");
    printf("│ Usuário          │ Tentativas       │ Status       │\n");
    printf("├──────────────────┼──────────────────┼──────────────┤\n");

    for (int i = 0; i < user_count; i++) {
        const char *status = "✅ Ativo";
        if (users[i].locked_until > time(NULL)) {
            status = "🔒 Bloqueado";
        }

        printf("│ %-16s │ %d/3             │ %-12s │\n",
               users[i].username,
               users[i].login_attempts,
               status);
    }
    printf("└──────────────────┴──────────────────┴──────────────┘\n");
}

/**
 * Menu interativo
 */
void mostrar_menu() {
    printf("\n");
    printf("╔═════════════════════════════════════════╗\n");
    printf("║     🔐 VALIDADOR DE SENHAS COM AGLE    ║\n");
    printf("╚═════════════════════════════════════════╝\n");
    printf("\n");
    printf("1. Registrar novo usuário\n");
    printf("2. Fazer login\n");
    printf("3. Listar usuários\n");
    printf("4. Sair\n");
    printf("\nEscolha: ");
}

int main(void) {
    AGLE_CTX ctx;
    int opcao;

    // Inicializar AGLE
    if (!AGLE_Init(&ctx)) {
        fprintf(stderr, "❌ Erro ao inicializar AGLE\n");
        return 1;
    }

    printf("\n╔═════════════════════════════════════════╗\n");
    printf("║   Sistema de Validação de Senhas AGLE  ║\n");
    printf("║                                         ║\n");
    printf("║  Usando Criptografia com SHAKE256      ║\n");
    printf("║  KDF: 100.000 iterações por senha      ║\n");
    printf("╚═════════════════════════════════════════╝\n");

    // Loop principal
    while (1) {
        mostrar_menu();
        if (scanf("%d", &opcao) != 1) {
            // Limpar buffer em caso de entrada inválida
            while (getchar() != '\n');
            printf("❌ Entrada inválida! Digite um número.\n");
            continue;
        }
        getchar(); // Limpar newline

        switch (opcao) {
            case 1: {
                // Registrar usuário
                char username[64];
                char password[256];

                printf("\n📝 REGISTRAR NOVO USUÁRIO\n");
                printf("┌─────────────────────────────┐\n");
                printf("Usuário: ");
                if (!fgets(username, sizeof(username), stdin)) {
                    printf("└─────────────────────────────┘\n");
                    printf("❌ Erro ao ler usuário!\n");
                    break;
                }
                username[strcspn(username, "\n")] = 0;

                printf("Senha (mínimo 8 caracteres): ");
                if (!fgets(password, sizeof(password), stdin)) {
                    printf("└─────────────────────────────┘\n");
                    printf("❌ Erro ao ler senha!\n");
                    break;
                }
                password[strcspn(password, "\n")] = 0;

                if (strlen(password) < 8) {
                    printf("└─────────────────────────────┘\n");
                    printf("❌ Senha muito curta! Mínimo 8 caracteres.\n");
                    break;
                }

                registrar_usuario(&ctx, username, password);
                printf("└─────────────────────────────┘\n");
                break;
            }

            case 2: {
                // Login
                char username[64];
                char password[256];

                printf("\n🔓 FAZER LOGIN\n");
                printf("┌─────────────────────────────┐\n");
                printf("Usuário: ");
                if (!fgets(username, sizeof(username), stdin)) {
                    printf("└─────────────────────────────┘\n");
                    printf("❌ Erro ao ler usuário!\n");
                    break;
                }
                username[strcspn(username, "\n")] = 0;

                printf("Senha: ");
                if (!fgets(password, sizeof(password), stdin)) {
                    printf("└─────────────────────────────┘\n");
                    printf("❌ Erro ao ler senha!\n");
                    break;
                }
                password[strcspn(password, "\n")] = 0;

                autenticar_usuario(&ctx, username, password);
                printf("└─────────────────────────────┘\n");
                break;
            }

            case 3: {
                // Listar usuários
                listar_usuarios();
                break;
            }

            case 4: {
                // Sair
                printf("\n👋 Até logo!\n");
                AGLE_Cleanup(&ctx);
                return 0;
            }

            default:
                printf("❌ Opção inválida!\n");
        }
    }

    return 0;
}
