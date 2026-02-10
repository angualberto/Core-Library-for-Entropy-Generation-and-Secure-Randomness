# AGLE Library - Complete Documentation

**AGLE** (Alpha-Gauss-Logistic Entropy Generator) - Biblioteca de Criptografia e Números Aleatórios para Senhas e Comunicação Segura

## Table of Contents

1. [Visao Geral](#visao-geral)
2. [Compilacao](#compilacao)
3. [API Reference](#api-reference)
4. [Exemplos de Uso](#exemplos-de-uso)
5. [Casos de Uso](#casos-de-uso)
6. [Seguranca](#seguranca)
    - [Threat Model e Fontes de Entropia](#threat-model-e-fontes-de-entropia)
    - [Modos de Operacao (Conceituais)](#modos-de-operacao-conceituais)
    - [Testes Estatisticos e Referencias](#testes-estatisticos-e-referencias)
7. [Troubleshooting](#troubleshooting)

---

## Visao Geral

AGLE é uma biblioteca C profissional para:

- **Geração de Números Aleatórios Criptográficos (CSPRNG)**
- **Geração Segura de Senhas**
- **Hashing com SHAKE256 (SHA-3)**
- **Derivação de Chaves (KDF)**
- **Geração de Tokens de Sessão**
- **Suporte a Comunicação Segura**

### Caracteristicas

- **Compilacao**: C99, GCC/Clang
- **Dependencias**: OpenSSL 3.0+
- **Seguranca**: Conforme NIST SP 800-22
- **Performance**: Otimizado com `-O3`
- **Licenca**: Apache-2.0

---

## Compilacao

### Requisitos

```bash
# Ubuntu/Debian
sudo apt-get install build-essential libssl-dev

# CentOS/RHEL
sudo yum install gcc openssl-devel

# macOS
brew install openssl
```

### Compilar Biblioteca Completa

```bash
# Construir tudo (static, shared e examples)
make all

# Apenas static library
make static

# Apenas shared library
make shared

# Apenas exemplos
make examples

# Executar exemplos
make run
```

### Compilar Aplicação com AGLE

#### Usando Static Library
```bash
gcc -o minha_app main.c agle.c -lssl -lcrypto -O3
```

#### Usando Shared Library
```bash
gcc -o minha_app main.c -L./build/lib -I. -lagle -lssl -lcrypto -O3
export LD_LIBRARY_PATH=./build/lib:$LD_LIBRARY_PATH
./minha_app
```

### Instalação no Sistema

```bash
# Instalar para /usr/local
sudo make install

# Depois, compilar aplicações com:
gcc -o minha_app main.c -lagle -lssl -lcrypto
```

---

## API Reference

### Estruturas

#### `AGLE_CTX` - Context
```c
typedef struct {
    uint8_t state[256];
    size_t position;
    int urandom_fd;
    uint8_t entropy_pool[4096];
} AGLE_CTX;
```

#### `AGLE_CharsetFlags` - Opções de Caracteres para Senha
```c
typedef enum {
    AGLE_CHARSET_LOWERCASE = 1,    /* a-z */
    AGLE_CHARSET_UPPERCASE = 2,    /* A-Z */
    AGLE_CHARSET_DIGITS = 4,       /* 0-9 */
    AGLE_CHARSET_SYMBOLS = 8,      /* !@#$%^&* */
    AGLE_CHARSET_ALL = 15          /* Todos */
} AGLE_CharsetFlags;
```

---

### Inicialização e Limpeza

#### `AGLE_Init()`
Inicializa o contexto AGLE. **DEVE ser chamado antes de usar qualquer outra função**.

```c
AGLE_CTX ctx;
if (!AGLE_Init(&ctx)) {
    printf("Erro ao inicializar AGLE\n");
    return 1;
}
```

#### `AGLE_Cleanup()`
Limpa dados sensíveis e libera recursos.

```c
AGLE_Cleanup(&ctx);
```

---

### Geração de Números Aleatórios

#### `AGLE_GetRandomBytes()`
Gera bytes aleatórios criptograficamente seguros.

```c
uint8_t random_bytes[32];
AGLE_GetRandomBytes(&ctx, random_bytes, 32);
```

#### `AGLE_GetRandomInt()`
Gera inteiro aleatório no range [0, max).

```c
uint32_t random_num;
AGLE_GetRandomInt(&ctx, 100, &random_num);  /* [0, 99] */
```

#### `AGLE_GetRandom64()`
Gera número aleatório de 64 bits.

```c
uint64_t random_64;
AGLE_GetRandom64(&ctx, &random_64);
```

---

### Geração de Senhas

#### `AGLE_GeneratePassword()`
Gera senha criptograficamente segura.

**Parâmetros:**
- `ctx`: Contexto AGLE
- `charset_flags`: Tipos de caracteres (LOW|UP|DIGITS|SYMBOLS)
- `length`: Comprimento (8-1024)
- `out`: Buffer de saída (length+1 bytes)

```c
char password[33];

/* Senha forte: letras, números e símbolos (32 caracteres) */
AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, password);
printf("Primeira senha: %s\n", password);

/* Senha média: letras e números (24 caracteres) */
AGLE_GeneratePassword(&ctx, 
    AGLE_CHARSET_UPPERCASE | AGLE_CHARSET_LOWERCASE | AGLE_CHARSET_DIGITS, 
    24, password);
printf("Segunda senha: %s\n", password);

/* Apenas minúsculas e números (16 caracteres) */
AGLE_GeneratePassword(&ctx, 
    AGLE_CHARSET_LOWERCASE | AGLE_CHARSET_DIGITS, 
    16, password);
printf("Terceira senha: %s\n", password);
```

#### `AGLE_GeneratePassphrase()`
Gera frase de senha (múltiplas palavras).

**Parâmetros:**
- `ctx`: Contexto AGLE
- `num_words`: Número de palavras (3-20)
- `separator`: Caractere separador ('-', ' ', '_', etc)
- `out`: Buffer de saída (min 512 bytes)

```c
char passphrase[512];

/* 5 palavras com hífen */
AGLE_GeneratePassphrase(&ctx, 5, '-', passphrase);
printf("Passphrase: %s\n", passphrase);
/* Exemplo: alpha-secure-cryptographic-random-shield */

/* 8 palavras com espaço */
AGLE_GeneratePassphrase(&ctx, 8, ' ', passphrase);
/* Exemplo: gamma lambda secure random alpha cipher beta omega */
```

---

### Hashing (SHAKE256)

#### `AGLE_HashSHAKE256()`
Faz hash de dados binários.

```c
const uint8_t data[] = "Hello, World!";
uint8_t hash[32];  /* 256 bits */

AGLE_HashSHAKE256(data, sizeof(data), hash, 32);
```

#### `AGLE_HashString()`
Faz hash de string.

```c
uint8_t hash[64];  /* 512 bits */
AGLE_HashString("password123", hash, 64);
```

#### `AGLE_HashSHAKE256_Hex()`
Retorna hash em formato hexadecimal.

```c
char hex_hash[65];  /* 256 bits = 32 bytes = 64 hex chars + '\0' */
AGLE_HashSHAKE256_Hex("text", 4, hex_hash, 32);
printf("Hash: %s\n", hex_hash);
```

---

### Derivação de Chaves (KDF)

#### `AGLE_DeriveKey()`
Deriva chave criptográfica de password + salt.

**Parâmetros:**
- `password`: Senha de entrada
- `password_len`: Tamanho senha
- `salt`: Salt aleatório
- `salt_len`: Tamanho salt
- `iterations`: Número iterações (recomendado: 100000)
- `key`: Buffer de saída
- `key_len`: Tamanho chave desejada

```c
const char *password = "MySecurePassword";
uint8_t salt[16];
uint8_t derived_key[32];

/* Gerar salt aleatório */
AGLE_GetRandomBytes(&ctx, salt, 16);

/* Derivar chave com 100.000 iterações */
AGLE_DeriveKey(
    (uint8_t*)password, strlen(password),
    salt, 16,
    100000,
    derived_key, 32
);

printf("Chave derivada (hex): ");
char hex_key[65];
AGLE_BytesToHex(derived_key, 32, hex_key);
printf("%s\n", hex_key);
```

---

### Tokens e Nonces

#### `AGLE_GenerateSessionToken()`
Gera token seguro para sessões.

```c
uint8_t token[32];
AGLE_GenerateSessionToken(&ctx, token, 32);

/* Converter para hex */
char hex_token[65];
AGLE_BytesToHex(token, 32, hex_token);
printf("Session token: %s\n", hex_token);
```

#### `AGLE_GenerateSessionTokenHex()`
Gera token diretamente em hex.

```c
char hex_token[65];
AGLE_GenerateSessionTokenHex(&ctx, hex_token, 32);
printf("Token: %s\n", hex_token);
```

#### `AGLE_GenerateNonce()`
Gera nonce (número único) para protocolos.

```c
uint8_t nonce[8];
uint64_t nonce_value = AGLE_GenerateNonce(&ctx, nonce);
printf("Nonce: %lu\n", nonce_value);
```

---

### Funções Utilitárias

#### `AGLE_BytesToHex()`
Converte bytes para string hexadecimal.

```c
uint8_t bytes[16] = {0x01, 0x23, 0x45, /* ... */ };
char hex[33];  /* 16 bytes * 2 + '\0' */

AGLE_BytesToHex(bytes, 16, hex);
printf("Hex: %s\n", hex);  /* "0123456789abcdef..." */
```

#### `AGLE_HexToBytes()`
Converte string hex para bytes.

```c
const char *hex = "48656c6c6f";
uint8_t bytes[10];

int len = AGLE_HexToBytes(hex, bytes, 10);
/* bytes agora contém: "Hello" */
```

#### `AGLE_SecureZero()`
Apaga dados sensíveis na memória.

```c
uint8_t password[128];
/* ... usar password ... */
AGLE_SecureZero(password, sizeof(password));  /* Seguro contra otimizações */
```

---

## Exemplos de Uso

### Exemplo 1: Gerar Senha para Usuário

```c
#include "agle.h"
#include <stdio.h>

int main(void) {
    AGLE_CTX ctx;
    
    if (!AGLE_Init(&ctx)) {
        printf("Erro ao inicializar\n");
        return 1;
    }

    char password[33];
    AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, password);
    
    printf("Nova senha para usuário: %s\n", password);
    
    AGLE_Cleanup(&ctx);
    return 0;
}
```

### Exemplo 2: Gerar Token de API

```c
#include "agle.h"
#include <stdio.h>

int main(void) {
    AGLE_CTX ctx;
    AGLE_Init(&ctx);

    /* Gerar chave de API de 32 bytes */
    char api_key[65];
    AGLE_GenerateSessionTokenHex(&ctx, api_key, 32);
    
    printf("API Key: %s\n", api_key);
    
    AGLE_Cleanup(&ctx);
    return 0;
}
```

### Exemplo 3: Hashing de Senha

```c
#include "agle.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    const char *password = "usuario123";
    char hash_hex[65];
    
    /* Fazer hash da senha */
    AGLE_HashSHAKE256_Hex(
        (uint8_t*)password, strlen(password),
        hash_hex, 32
    );
    
    printf("Senha hash: %s\n", hash_hex);
    return 0;
}
```

### Exemplo 4: Derivação de Chave para Criptografia

```c
#include "agle.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    AGLE_CTX ctx;
    AGLE_Init(&ctx);

    const char *master_password = "MyMasterPassword123!";
    uint8_t salt[16];
    uint8_t encryption_key[32];
    
    /* Gerar salt aleatório */
    AGLE_GetRandomBytes(&ctx, salt, 16);
    
    /* Derivar chave com 100k iterações */
    AGLE_DeriveKey(
        (uint8_t*)master_password, strlen(master_password),
        salt, 16,
        100000,
        encryption_key, 32
    );
    
    char hex_key[65];
    AGLE_BytesToHex(encryption_key, 32, hex_key);
    printf("Chave de criptografia: %s\n", hex_key);
    
    AGLE_SecureZero(&encryption_key, sizeof(encryption_key));
    AGLE_Cleanup(&ctx);
    return 0;
}
```

---

## Casos de Uso

### 1. Sistema de Gerenciamento de Senhas
```c
/* Gerar senhas únicas para cada serviço */
for (int i = 0; i < 10; i++) {
    char pwd[33];
    AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, pwd);
    printf("Serviço %d: %s\n", i+1, pwd);
}
```

### 2. REST API com Token de Autenticação
```c
/* Headers HTTP */
char token[65];
AGLE_GenerateSessionTokenHex(&ctx, token, 32);
printf("Authorization: Bearer %s\n", token);
```

### 3. Banco de Dados com Chaves Derivadas
```c
/* Armazenar apenas hash da senha, não a senha! */
uint8_t stored_salt[16];
uint8_t stored_hash[32];

AGLE_GetRandomBytes(&ctx, stored_salt, 16);

AGLE_DeriveKey(
    user_password, strlen(user_password),
    stored_salt, 16,
    100000,
    stored_hash, 32
);

/* Salvar (stored_salt, stored_hash) no DB */
```

### 4. Comunicação Segura
```c
/* Gerar nonce para cada mensagem */
uint8_t nonce[8];
uint64_t nonce_value = AGLE_GenerateNonce(&ctx, nonce);

/* Usar em protocolo: [nonce || ciphertext || tag] */
```

---

## Seguranca

### Threat Model e Fontes de Entropia

AGLE **nao substitui o OpenSSL**. A biblioteca utiliza primitivas criptograficas do OpenSSL (por exemplo, SHAKE256) e foca em **orquestrar entropia**, **combinar fontes** e **amplificar variacao** para usos como CSPRNG, senhas e tokens.

Fontes tipicas de entropia:
- `/dev/urandom` (sistema operacional)
- Utilitarios criptograficos do OpenSSL (hashes e expansao)

AGLE adiciona um **pipeline de mistura, estado e feedback** para tornar o uso dessas fontes **mais controlado e reprodutivel**, especialmente quando acoplado a experimentos com mapas caoticos.

### Modos de Operacao (Conceituais)

Mesmo que a implementacao interna use o mesmo pipeline, a documentacao distingue dois modos **conceituais**:

- `AGLE_MODE_SYSTEM_ENTROPY`
    - Foco em `/dev/urandom` + utilitarios criptograficos.
    - Ideal para uso geral em aplicacoes e servicos.

- `AGLE_MODE_CHAOTIC_FEEDBACK`
    - Quando o AGLE e integrado a um mapa caotico externo.
    - Conecta diretamente o pipeline de entropia aos experimentos descritos nos artigos.

### Testes Estatisticos e Referencias

Resultados estatisticos (Dieharder, NIST, RGB) foram obtidos no projeto e sao referenciados no **preprint associado**. Para auditoria cientifica, use o preprint do projeto (DOI/URL a ser publicado).

### Boas Práticas

1. **Sempre inicializar contexto**
   ```c
   AGLE_CTX ctx;
   if (!AGLE_Init(&ctx)) { /* erro */ }
   ```

2. **Limpar dados sensíveis**
   ```c
   uint8_t key[32];
   /* ... usar key ... */
   AGLE_SecureZero(key, 32);  /* ← importante! */
   ```

3. **Usar comprimentos adequados**
   - Senhas: mínimo 16 caracteres
   - Salts: mínimo 16 bytes
   - Hashes: mínimo 32 bytes
   - Tokens: mínimo 32 bytes

4. **Iterações de KDF**
   - Desenvolvimento: 10.000
   - Produção: 100.000+
   - Crítico: 1.000.000+

5. **Nunca reutilize salts**
   ```c
   uint8_t salt[16];
   AGLE_GetRandomBytes(&ctx, salt, 16);  /* novo salt cada vez */
   ```

---

## Troubleshooting

### "Error: libssl.so not found"
```bash
# Ubuntu/Debian
sudo apt-get install libssl-dev

# Ou compilar com rpath
gcc -o app main.c -Wl,-rpath,/usr/local/lib -L/usr/local/lib -lagle
```

### "/dev/urandom not found"
- Sistema POSIX/Unix apenas
- Não funciona no Windows (use WSL ou implementação alternativa)

### "Segmentation fault"
- Verificar se `AGLE_Init()` foi chamado
- Verificar tamanhos de buffer
- Usar `valgrind` para debug:
  ```bash
  valgrind ./agle_examples
  ```

### Senhas fracas
- Use `AGLE_CHARSET_ALL` para máxima segurança
- Mínimo 16 caracteres para senhas normais
- Use passfrases para maior memorabilidade

---

## Performance

Benchmark típico (Intel i7, Linux):

| Operação | Tempo |
|----------|-------|
| `AGLE_Init()` | ~5ms |
| `AGLE_GetRandomBytes(1KB)` | ~2ms |
| `AGLE_GeneratePassword(32)` | ~3ms |
| `AGLE_HashSHAKE256(1KB→32B)` | <1ms |
| `AGLE_DeriveKey(100k iter)` | ~1s |

---

## Licenca

Apache-2.0

---

## Suporte

Para dúvidas ou bugs:
1. Verificar [README.md](README.md)
2. Consultar `agle_examples.c` para exemplos completos
3. Rodar `make help` para listar todos os targets

---

**AGLE Library v2.0.0** | Geração Segura de Criptografia e Números Aleatórios
