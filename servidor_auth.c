/* 
 * SERVIDOR DE AUTENTICAÇÃO SEGURA
 * Protocolo HTTP com API REST usando biblioteca AGLE
 */

#include "agle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>

#define PORT 8080
#define MAX_USERS 100
#define MAX_SESSIONS 100
#define SESSION_TIMEOUT 3600  // 1 hora

// Comparação constant-time para prevenir timing attacks
bool constant_time_compare(const uint8_t *a, const uint8_t *b, size_t len) {
    volatile uint8_t result = 0;
    for (size_t i = 0; i < len; i++) {
        result |= a[i] ^ b[i];
    }
    return result == 0;
}

// Estrutura de usuário
typedef struct {
    char username[64];
    uint8_t salt[16];
    uint8_t password_hash[32];
    bool active;
    int failed_attempts;
    time_t locked_until;
} User;

// Estrutura de sessão
typedef struct {
    char token[65];
    char username[64];
    time_t created_at;
    time_t expires_at;
    bool valid;
} Session;

// Dados globais
static AGLE_CTX ctx;
static User users[MAX_USERS];
static Session sessions[MAX_SESSIONS];
static int user_count = 0;
static int session_count = 0;

// ═══════════════════════════════════════════════════════════
//                    FUNÇÕES DE USUÁRIO
// ═══════════════════════════════════════════════════════════

User* encontrar_usuario(const char *username) {
    for (int i = 0; i < user_count; i++) {
        if (users[i].active && strcmp(users[i].username, username) == 0) {
            return &users[i];
        }
    }
    return NULL;
}

bool registrar_usuario(const char *username, const char *password) {
    if (user_count >= MAX_USERS || encontrar_usuario(username) != NULL) {
        return false;
    }
    
    // VALIDAÇÃO DE SENHA FORTE
    size_t pass_len = strlen(password);
    if (pass_len < 12) {
        printf("❌ Senha muito curta (mínimo 12 caracteres)\n");
        return false;
    }
    if (pass_len > 240) {
        printf("❌ Senha muito longa (máximo 240 caracteres)\n");
        return false;
    }
    
    // Verificar complexidade
    bool tem_maiuscula = false, tem_minuscula = false;
    bool tem_numero = false, tem_simbolo = false;
    for (size_t i = 0; i < pass_len; i++) {
        if (isupper(password[i])) tem_maiuscula = true;
        if (islower(password[i])) tem_minuscula = true;
        if (isdigit(password[i])) tem_numero = true;
        if (!isalnum(password[i])) tem_simbolo = true;
    }
    if (!(tem_maiuscula && tem_minuscula && tem_numero && tem_simbolo)) {
        printf("❌ Senha deve ter: maiúscula, minúscula, número e símbolo\n");
        return false;
    }
    
    User *user = &users[user_count];
    strncpy(user->username, username, sizeof(user->username) - 1);
    user->active = true;
    user->failed_attempts = 0;
    user->locked_until = 0;
    
    // Gerar salt aleatório
    AGLE_GetRandomBytes(&ctx, user->salt, 16);
    
    // Criar buffer: password + salt (criptografia determinística)
    uint8_t combined[256];
    memcpy(combined, password, pass_len);
    memcpy(combined + pass_len, user->salt, 16);
    
    // Usar SHAKE256 direto (100% determinístico)
    AGLE_HashSHAKE256(combined, pass_len + 16, user->password_hash, 32);
    
    // ZEROIZAR SENHA DA MEMÓRIA
    memset(combined, 0, sizeof(combined));
    
    user_count++;
    
    // LOG SEGURO (SEM SALT)
    printf("✅ Usuário registrado: %s\n", username);
    
    return true;
}

bool validar_senha(User *user, const char *password) {
    // VERIFICAR BLOQUEIO POR RATE LIMITING
    time_t now = time(NULL);
    if (now < user->locked_until) {
        int tempo_restante = (int)(user->locked_until - now);
        printf("🔒 Conta bloqueada: %s (aguarde %d segundos)\n", 
               user->username, tempo_restante);
        return false;
    }
    
    // Recriar a mesma combinação: password + salt
    size_t pass_len = strlen(password);
    uint8_t combined[256];
    uint8_t hash[32];
    
    memcpy(combined, password, pass_len);
    memcpy(combined + pass_len, user->salt, 16);
    
    // Gerar hash com SHAKE256 (determinístico)
    AGLE_HashSHAKE256(combined, pass_len + 16, hash, 32);
    
    // ZEROIZAR SENHA DA MEMÓRIA
    memset(combined, 0, sizeof(combined));
    
    // USAR COMPARAÇÃO CONSTANT-TIME
    bool resultado = constant_time_compare(hash, user->password_hash, 32);
    
    if (resultado) {
        // SUCESSO: Resetar contadores
        user->failed_attempts = 0;
        user->locked_until = 0;
        printf("✅ Login bem-sucedido: %s\n", user->username);
    } else {
        // FALHA: Incrementar tentativas
        user->failed_attempts++;
        printf("❌ Falha de login: %s (tentativa %d)\n", 
               user->username, user->failed_attempts);
        
        // RATE LIMITING EXPONENCIAL
        if (user->failed_attempts >= 5) {
            user->locked_until = now + 900;  // 15 minutos
            printf("🔒 Conta bloqueada por 15 minutos após 5 tentativas\n");
        } else if (user->failed_attempts >= 3) {
            user->locked_until = now + 300;  // 5 minutos
            printf("🔒 Conta bloqueada por 5 minutos após 3 tentativas\n");
        }
    }
    
    return resultado;
}

// ═══════════════════════════════════════════════════════════
//                    FUNÇÕES DE SESSÃO
// ═══════════════════════════════════════════════════════════

void limpar_sessoes_expiradas() {
    time_t now = time(NULL);
    int removidas = 0;
    
    for (int i = 0; i < session_count; i++) {
        if (sessions[i].expires_at < now || !sessions[i].valid) {
            // Zeroizar dados da sessão
            memset(&sessions[i], 0, sizeof(Session));
            
            // Mover última sessão para esta posição
            if (i < session_count - 1) {
                sessions[i] = sessions[session_count - 1];
                i--;  // Reprocessar esta posição
            }
            session_count--;
            removidas++;
        }
    }
    
    if (removidas > 0) {
        printf("🧹 %d sessões expiradas limpas\n", removidas);
    }
}

char* criar_sessao(const char *username) {
    // LIMPAR SESSÕES EXPIRADAS ANTES DE CRIAR NOVA
    limpar_sessoes_expiradas();
    
    if (session_count >= MAX_SESSIONS) {
        return NULL;
    }
    
    Session *sess = &sessions[session_count];
    
    // Gerar token único
    AGLE_GenerateSessionTokenHex(&ctx, sess->token, 32);
    strncpy(sess->username, username, sizeof(sess->username) - 1);
    sess->created_at = time(NULL);
    sess->expires_at = sess->created_at + SESSION_TIMEOUT;
    sess->valid = true;
    
    session_count++;
    printf("✅ Sessão criada para: %s (expira em 1h)\n", username);
    return sess->token;
}

Session* validar_token(const char *token) {
    time_t now = time(NULL);
    size_t token_len = strlen(token);
    
    for (int i = 0; i < session_count; i++) {
        // USAR COMPARAÇÃO CONSTANT-TIME PARA TOKENS
        if (sessions[i].valid && 
            strlen(sessions[i].token) == token_len &&
            constant_time_compare((uint8_t*)sessions[i].token, 
                                 (uint8_t*)token, 
                                 token_len)) {
            
            if (sessions[i].expires_at > now) {
                return &sessions[i];
            } else {
                // Sessão expirada
                sessions[i].valid = false;
                printf("⏰ Sessão expirada: %s\n", sessions[i].username);
                return NULL;
            }
        }
    }
    return NULL;
}

void invalidar_sessao(const char *token) {
    for (int i = 0; i < session_count; i++) {
        if (strcmp(sessions[i].token, token) == 0) {
            sessions[i].valid = false;
            printf("🚪 Logout: %s\n", sessions[i].username);
            return;
        }
    }
}

// ═══════════════════════════════════════════════════════════
//                    PROTOCOLO HTTP/REST
// ═══════════════════════════════════════════════════════════

void extrair_campo(const char *body, const char *campo, char *destino, size_t max_len) {
    char padrao[128];
    snprintf(padrao, sizeof(padrao), "\"%s\":\"", campo);
    
    const char *inicio = strstr(body, padrao);
    if (inicio) {
        inicio += strlen(padrao);
        const char *fim = strchr(inicio, '"');
        if (fim) {
            size_t len = fim - inicio;
            if (len >= max_len) len = max_len - 1;
            strncpy(destino, inicio, len);
            destino[len] = '\0';
        }
    }
}

void enviar_resposta(int client_socket, int status, const char *json_body) {
    char response[4096];
    const char *status_text = (status == 200) ? "OK" : 
                              (status == 401) ? "Unauthorized" : 
                              (status == 400) ? "Bad Request" : "Error";
    
    snprintf(response, sizeof(response),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s", status, status_text, json_body);
    
    ssize_t written = write(client_socket, response, strlen(response));
    (void)written;  // Ignorar intencionalmente
}

void processar_requisicao(int client_socket, const char *request) {
    char method[16], path[256], body[2048];
    char username[64] = {0}, password[128] = {0}, token[128] = {0};
    
    // Parsear método e path
    sscanf(request, "%s %s", method, path);
    
    // Extrair body (após \r\n\r\n)
    const char *body_start = strstr(request, "\r\n\r\n");
    if (body_start) {
        strncpy(body, body_start + 4, sizeof(body) - 1);
    } else {
        body[0] = '\0';
    }
    
    printf("\n📨 %s %s\n", method, path);
    
    // OPTIONS (CORS preflight)
    if (strcmp(method, "OPTIONS") == 0) {
        enviar_resposta(client_socket, 200, "{}");
        return;
    }
    
    // POST /register
    if (strcmp(path, "/register") == 0) {
        extrair_campo(body, "username", username, sizeof(username));
        extrair_campo(body, "password", password, sizeof(password));
        
        if (strlen(username) == 0 || strlen(password) < 8) {
            enviar_resposta(client_socket, 400, 
                "{\"success\":false,\"error\":\"Dados inválidos\"}");
            return;
        }
        
        if (registrar_usuario(username, password)) {
            enviar_resposta(client_socket, 200,
                "{\"success\":true,\"message\":\"Usuário registrado!\"}");
        } else {
            enviar_resposta(client_socket, 400,
                "{\"success\":false,\"error\":\"Usuário já existe\"}");
        }
        return;
    }
    
    // POST /login
    if (strcmp(path, "/login") == 0) {
        extrair_campo(body, "username", username, sizeof(username));
        extrair_campo(body, "password", password, sizeof(password));
        
        User *user = encontrar_usuario(username);
        if (user && validar_senha(user, password)) {
            char *session_token = criar_sessao(username);
            char json[512];
            snprintf(json, sizeof(json),
                "{\"success\":true,\"token\":\"%s\",\"username\":\"%s\"}",
                session_token, username);
            enviar_resposta(client_socket, 200, json);
        } else {
            enviar_resposta(client_socket, 401,
                "{\"success\":false,\"error\":\"Credenciais inválidas\"}");
        }
        return;
    }
    
    // GET /validate
    if (strcmp(path, "/validate") == 0) {
        // Extrair token do header Authorization
        const char *auth_header = strstr(request, "Authorization: Bearer ");
        if (auth_header) {
            sscanf(auth_header + 22, "%64s", token);
        }
        
        Session *sess = validar_token(token);
        if (sess) {
            char json[512];
            time_t tempo_restante = sess->expires_at - time(NULL);
            snprintf(json, sizeof(json),
                "{\"success\":true,\"username\":\"%s\",\"expires_in\":%ld}",
                sess->username, tempo_restante);
            enviar_resposta(client_socket, 200, json);
        } else {
            enviar_resposta(client_socket, 401,
                "{\"success\":false,\"error\":\"Token inválido ou expirado\"}");
        }
        return;
    }
    
    // POST /logout
    if (strcmp(path, "/logout") == 0) {
        extrair_campo(body, "token", token, sizeof(token));
        invalidar_sessao(token);
        enviar_resposta(client_socket, 200,
            "{\"success\":true,\"message\":\"Logout realizado\"}");
        return;
    }
    
    // GET /stats
    if (strcmp(path, "/stats") == 0) {
        char json[512];
        snprintf(json, sizeof(json),
            "{\"users\":%d,\"sessions\":%d,\"active_sessions\":%d}",
            user_count, session_count, session_count);  // Simplificado
        enviar_resposta(client_socket, 200, json);
        return;
    }
    
    // GET / (rota raiz)
    if (strcmp(path, "/") == 0) {
        enviar_resposta(client_socket, 200,
            "{\"status\":\"online\",\"message\":\"Servidor de Autenticação AGLE\"}");
        return;
    }
    
    // Ignorar favicon (não é erro)
    if (strstr(path, "favicon.ico") != NULL) {
        enviar_resposta(client_socket, 404, "{\"error\":\"Not found\"}");
        return;
    }
    
    // 404 - Rota não encontrada
    enviar_resposta(client_socket, 400, "{\"error\":\"Rota não encontrada\"}");
}

// ═══════════════════════════════════════════════════════════
//                    SERVIDOR HTTP
// ═══════════════════════════════════════════════════════════

void iniciar_servidor(void) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[4096];
    
    // Criar socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("❌ Erro ao criar socket");
        exit(1);
    }
    
    // Permitir reutilização de porta
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Configurar endereço
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    // Bind
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("❌ Erro no bind");
        exit(1);
    }
    
    // Listen
    if (listen(server_socket, 10) < 0) {
        perror("❌ Erro no listen");
        exit(1);
    }
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║      🔐 SERVIDOR DE AUTENTICAÇÃO SEGURA ATIVO! 🔐       ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("🌐 Servidor rodando em: http://localhost:%d\n", PORT);
    printf("\n");
    printf("📡 ENDPOINTS DISPONÍVEIS:\n");
    printf("   POST /register  - Registrar novo usuário\n");
    printf("   POST /login     - Fazer login e obter token\n");
    printf("   GET  /validate  - Validar token (header Authorization)\n");
    printf("   POST /logout    - Encerrar sessão\n");
    printf("   GET  /stats     - Estatísticas do servidor\n");
    printf("\n");
    printf("💡 Abra cliente_auth.html no navegador para usar!\n");
    printf("   Ou use: firefox cliente_auth.html\n");
    printf("\n");
    printf("⏹️  Pressione Ctrl+C para parar o servidor\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    // Loop principal
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            continue;
        }
        
        // Ler requisição
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
        if (bytes_read < 0) continue;
        
        // Processar
        processar_requisicao(client_socket, buffer);
        
        // Fechar conexão
        close(client_socket);
    }
    
    close(server_socket);
}

// ═══════════════════════════════════════════════════════════
//                         MAIN
// ═══════════════════════════════════════════════════════════

int main(void) {
    // Inicializar AGLE
    if (!AGLE_Init(&ctx)) {
        fprintf(stderr, "❌ Erro ao inicializar AGLE!\n");
        return 1;
    }
    
    // Inicializar servidor
    iniciar_servidor();
    
    // Cleanup
    AGLE_Cleanup(&ctx);
    return 0;
}
