# AGLE - Cheat Sheet (Referência Rápida)

## ⚡ Começar em 30 segundos

```c
#include "agle.h"

int main() {
    AGLE_CTX ctx;
    AGLE_Init(&ctx);
    
    // Gerar senha
    char password[33];
    AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, password);
    printf("Senha: %s\n", password);
    
    AGLE_Cleanup(&ctx);
    return 0;
}
```

**Compilar:**
```bash
gcc -o exemplo exemplo.c -lagle -lssl -lcrypto
```

---

## 🔐 Funções Principais (Copiar & Colar)

### 1️⃣ Inicializar
```c
AGLE_CTX ctx;
AGLE_Init(&ctx);           // ✅ Sempre fazer isso primeiro!

// ... use as funções ...

AGLE_Cleanup(&ctx);        // ✅ Sempre fazer isso por último!
```

### 2️⃣ Gerar Números Aleatórios

```c
// Bytes aleatórios
uint8_t bytes[16];
AGLE_GetRandomBytes(&ctx, bytes, 16);

// Inteiro (0-100)
uint32_t numero = AGLE_GetRandomInt(&ctx, 101);  // 0-100

// Número grande (64-bit)
uint64_t grande = AGLE_GetRandom64(&ctx);
```

### 3️⃣ Gerar Senhas

```c
char senha[33];

// Forte (todos caracteres)
AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, senha);
// Resultado: ")BJ29FdZ2J$uRQMo0E]T$wD)Y8/Akz6L"

// Só letras e números
AGLE_GeneratePassword(&ctx, 
    AGLE_CHARSET_UPPERCASE | AGLE_CHARSET_DIGITS, 16, senha);
// Resultado: "A7B2C9D1E4F6G8H3"

// Limpeza segura (IMPORTANTE!)
AGLE_SecureZero(senha, sizeof(senha));
```

**Opções de charset:**
- `AGLE_CHARSET_LOWERCASE` = letras minúsculas
- `AGLE_CHARSET_UPPERCASE` = letras maiúsculas  
- `AGLE_CHARSET_DIGITS` = números 0-9
- `AGLE_CHARSET_SYMBOLS` = !@#$%^&*()_+-=[]{}...
- `AGLE_CHARSET_ALL` = todos acima

### 4️⃣ Gerar Passphrases (Frases Memoráveis)

```c
char frase[256];

// Frase de 5 palavras
AGLE_GeneratePassphrase(&ctx, 5, "-", frase, sizeof(frase));
// Resultado: "desert-cipher-moon-rho-desert"

// Frase de 8 palavras com "_"
AGLE_GeneratePassphrase(&ctx, 8, "_", frase, sizeof(frase));
// Resultado: "alpha_beta_gamma_delta_epsilon_zeta_eta_theta"
```

### 5️⃣ Hash (SHAKE256)

```c
char hash[65];  // Para 256-bit (64 chars hex)

// Hash de string
AGLE_HashSHAKE256("senha123", 9, 256, hash);
printf("Hash: %s\n", hash);
// Resultado: "abc123def456..."

// Hash de arquivo
AGLE_HashString("dados", 5, 512, hash);  // 512-bit output
```

### 6️⃣ Derivar Chave (KDF) - Para Autenticação

```c
uint8_t salt[16];
uint8_t chave[32];
char chave_hex[65];

// 1. Gerar salt aleatório
AGLE_GetRandomBytes(&ctx, salt, 16);

// 2. Derivar chave da senha
AGLE_DeriveKey(&ctx, "senha_usuario", 13, salt, 16, 
               100000,    // Iterações (quanto mais = mais seguro mas lento)
               chave);

// 3. Converter para hex (opcional, para armazenar)
AGLE_BytesToHex(chave, 32, chave_hex);
printf("Chave: %s\n", chave_hex);
```

### 7️⃣ Gerar Tokens (Para APIs/Sessões)

```c
char token[65];     // 64 chars hex + null

// Token de sessão
AGLE_GenerateSessionTokenHex(&ctx, token);
printf("Token: %s\n", token);
// Resultado: "54aad6619000b5a557c27f686e512163102f0c6a73ff355e328df997592a1e6e"

// Nonce (número único para cada uso)
uint64_t nonce = AGLE_GenerateNonce(&ctx);
printf("Nonce: %llu\n", nonce);
```

### 8️⃣ Limpeza Segura (IMPORTANTE!)

```c
char senha[33] = "MinhaSenh@123";
AGLE_SecureZero(senha, sizeof(senha));  // Remove da memória
```

---

## 📚 Uso Real - Exemplos Completos

### Exemplo: Sistema de Login

```c
#include "agle.h"
#include <stdio.h>
#include <string.h>

int main() {
    AGLE_CTX ctx;
    AGLE_Init(&ctx);
    
    // 1. REGISTRO - Usuário cria conta
    printf("=== REGISTRO ===\n");
    char senha_usuario[100] = "MinhaSenha@123";
    uint8_t salt[16];
    uint8_t hash_armazenado[32];
    
    AGLE_GetRandomBytes(&ctx, salt, 16);  // Gerar salt
    AGLE_DeriveKey(&ctx, senha_usuario, strlen(senha_usuario), 
                   salt, 16, 100000, hash_armazenado);
    
    printf("✅ Usuário registrado com salt e hash\n");
    
    // 2. LOGIN - Usuário faz login
    printf("\n=== LOGIN ===\n");
    char senha_tentativa[100] = "MinhaSenha@123";
    uint8_t hash_tentativa[32];
    
    AGLE_DeriveKey(&ctx, senha_tentativa, strlen(senha_tentativa),
                   salt, 16, 100000, hash_tentativa);
    
    if (memcmp(hash_armazenado, hash_tentativa, 32) == 0) {
        printf("✅ Login bem-sucedido!\n");
        
        // 3. GERAR TOKEN - Criar sessão
        char token[65];
        AGLE_GenerateSessionTokenHex(&ctx, token);
        printf("Token de sessão: %s\n", token);
    } else {
        printf("❌ Senha incorreta\n");
    }
    
    AGLE_Cleanup(&ctx);
    return 0;
}
```

### Exemplo: Gerador de Senhas para Usuários

```c
#include "agle.h"
#include <stdio.h>

int main() {
    AGLE_CTX ctx;
    AGLE_Init(&ctx);
    
    printf("=== Gerar 5 Senhas Aleatórias ===\n");
    
    char senha[33];
    for (int i = 1; i <= 5; i++) {
        AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, senha);
        printf("Usuário %d: %s\n", i, senha);
        AGLE_SecureZero(senha, sizeof(senha));
    }
    
    AGLE_Cleanup(&ctx);
    return 0;
}
```

### Exemplo: API Key Generation

```c
#include "agle.h"
#include <stdio.h>

int main() {
    AGLE_CTX ctx;
    AGLE_Init(&ctx);
    
    printf("=== Gerar 3 API Keys ===\n");
    
    char api_key[65];
    for (int i = 1; i <= 3; i++) {
        AGLE_GenerateSessionTokenHex(&ctx, api_key);
        printf("App %d API Key: %s\n", i, api_key);
    }
    
    AGLE_Cleanup(&ctx);
    return 0;
}
```

---

## 🎯 Padrões Rápidos (Copy-Paste)

### Padrão 1: Gerar Senha Forte
```c
AGLE_CTX ctx;
AGLE_Init(&ctx);
char pwd[33];
AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, pwd);
// Use pwd aqui
AGLE_SecureZero(pwd, sizeof(pwd));
AGLE_Cleanup(&ctx);
```

### Padrão 2: Autenticar com Senha
```c
AGLE_CTX ctx;
AGLE_Init(&ctx);

uint8_t salt[16];
uint8_t hash1[32], hash2[32];

// Primeiro: armazenar
AGLE_GetRandomBytes(&ctx, salt, 16);
AGLE_DeriveKey(&ctx, "senha", 5, salt, 16, 100000, hash1);
// Salvar salt + hash1 no banco

// Depois: validar
AGLE_DeriveKey(&ctx, "senha", 5, salt, 16, 100000, hash2);
if (memcmp(hash1, hash2, 32) == 0) printf("OK\n");

AGLE_Cleanup(&ctx);
```

### Padrão 3: Gerar Token de Sessão
```c
AGLE_CTX ctx;
AGLE_Init(&ctx);
char token[65];
AGLE_GenerateSessionTokenHex(&ctx, token);
// Usar token em cookie/header
AGLE_Cleanup(&ctx);
```

---

## ⚠️ Regras de Segurança

✅ **SEMPRE FAZER:**
- Chamar `AGLE_Init()` antes de usar
- Chamar `AGLE_Cleanup()` no final
- Usar `AGLE_SecureZero()` para senhas
- Usar salts aleatórios diferentes para cada usuário
- Usar 100k+ iterações na KDF

❌ **NUNCA FAZER:**
- Hardcodear senhas
- Não limpar variáveis sensíveis
- Reutilizar o mesmo salt
- Usar menos de 100k iterações na KDF
- Armazenar senhas em texto plano

---

## 🔧 Troubleshooting Rápido

| Problema | Solução |
|----------|---------|
| `error: openssl/evp.h: not found` | `sudo apt-get install libssl-dev` |
| `undefined reference to 'AGLE_*'` | Adicionar `-lagle -lssl -lcrypto` no gcc |
| Programa para de responder | Aumentou iterações KDF? É normal (~1s por 100k) |
| Compiler warning | Use C99: `gcc -std=c99 ...` |
| Segfault | Chamar `AGLE_Init()` antes? |

---

## 📖 Próximos Passos

1. ✅ Copiar um exemplo acima
2. ✅ Compilar: `gcc -o teste teste.c -lagle -lssl -lcrypto`
3. ✅ Executar: `./teste`
4. ✅ Adaptar para seu projeto
5. ✅ Ler LIBRARY.md para detalhes completos

---

## 🚀 Comandos Práticos

```bash
# Compilar seu código com AGLE
gcc -o meu_programa meu_programa.c -lagle -lssl -lcrypto

# Ou com debug
gcc -g -o meu_programa meu_programa.c -lagle -lssl -lcrypto

# Ou com máxima otimização
gcc -O3 -march=native -o meu_programa meu_programa.c -lagle -lssl -lcrypto

# Testar se compila (sem executar)
gcc -c meu_programa.c -Wall -Wextra

# Checar symbol table
nm -C meu_programa
```

---

## 📞 Precisa de Mais?

- Função não listada? → Veja `LIBRARY.md`
- Como integrar? → Veja `INTEGRATION_GUIDE.md`
- Começar do zero? → Veja `QUICK_START.md`
- Navegar tudo? → Veja `INDEX.md`

---

**Status:** ✅ AGLE v2.0.0 Pronta para Usar
**Última atualização:** 9 de Fevereiro de 2026
