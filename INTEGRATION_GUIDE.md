# 🔧 GUIA DE INTEGRAÇÃO DA BIBLIOTECA AGLE

## 📋 Índice
1. [Como Copiar os Arquivos](#como-copiar)
2. [Integração em Seu Projeto](#integração)
3. [Exemplos de Integração](#exemplos)
4. [Troubleshooting](#troubleshooting)
5. [Checklist de Segurança](#checklist)

---

## 📁 Como Copiar os Arquivos {#como-copiar}

### Opção 1: Copiar para Seu Projeto (Recomendado)

```bash
# 1. Navegue até seu projeto
cd /seu/projeto/

# 2. Copie os arquivos AGLE
cp /home/andr/Documentos/.../agle.h .
cp /home/andr/Documentos/.../agle.c .

# 3. Seu projeto agora tem:
ls -la agle.*
# agle.h      (11 KB)
# agle.c      (11 KB)
```

### Opção 2: Usar como Subdiretório

```bash
# Criar subdiretório crypto/
mkdir -p libs/crypto
cp /home/andr/Documentos/.../agle.h libs/crypto/
cp /home/andr/Documentos/.../agle.c libs/crypto/

# Ao compilar:
gcc -o app main.c libs/crypto/agle.c -Ilibs/crypto -lssl -lcrypto
```

### Opção 3: Instalação Global no Sistema

```bash
cd /home/andr/Documentos/.../
sudo make install

# Depois, em qualquer projeto:
gcc -o app main.c -lagle -lssl -lcrypto
# Sem precisar de -I ou -L!
```

---

## 🛠️ Integração em Seu Projeto {#integração}

### Estrutura Recomendada

```
seu_projeto/
├── src/
│   ├── main.c
│   ├── app.c
│   └── ...
├── include/
│   ├── agle.h          ← Copie aqui
│   └── ...
├── lib/
│   ├── agle.c          ← Copie aqui
│   └── ...
├── build/
│   └── (artefatos de compilação)
├── Makefile
└── ...
```

### Makefile Exemplo para Seu Projeto

```makefile
# Seu Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O3 -Iinclude
LDFLAGS = -lssl -lcrypto

# Arquivos
SOURCES = src/main.c src/app.c lib/agle.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = meu_app

# Compilação
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
```

---

## 💻 Exemplos de Integração {#exemplos}

### Exemplo 1: App Simples

**arquivo: main.c**
```c
#include "agle.h"
#include <stdio.h>

int main(void) {
    AGLE_CTX ctx;
    
    if (!AGLE_Init(&ctx)) {
        fprintf(stderr, "Erro ao inicializar AGLE\n");
        return 1;
    }
    
    /* Sua lógica aqui */
    char password[33];
    AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, password);
    printf("Senha: %s\n", password);
    
    AGLE_Cleanup(&ctx);
    return 0;
}
```

**Compilar:**
```bash
gcc -o app main.c agle.c -lssl -lcrypto -O3 -Wall
```

---

### Exemplo 2: Biblioteca com AGLE

**arquivo: crypto.h**
```c
#ifndef CRYPTO_H
#define CRYPTO_H

#include "agle.h"
#include <stdint.h>

/* Sua interface customizada */
typedef struct {
    AGLE_CTX agle_ctx;
} CryptoManager;

CryptoManager* CryptoManager_Create(void);
void CryptoManager_GeneratePassword(CryptoManager *mgr, char *pwd);
void CryptoManager_Destroy(CryptoManager *mgr);

#endif
```

**arquivo: crypto.c**
```c
#include "crypto.h"
#include <stdlib.h>

CryptoManager* CryptoManager_Create(void) {
    CryptoManager *mgr = malloc(sizeof(CryptoManager));
    AGLE_Init(&mgr->agle_ctx);
    return mgr;
}

void CryptoManager_GeneratePassword(CryptoManager *mgr, char *pwd) {
    AGLE_GeneratePassword(&mgr->agle_ctx, AGLE_CHARSET_ALL, 32, pwd);
}

void CryptoManager_Destroy(CryptoManager *mgr) {
    AGLE_Cleanup(&mgr->agle_ctx);
    free(mgr);
}
```

**arquivo: main.c**
```c
#include "crypto.h"
#include <stdio.h>

int main(void) {
    CryptoManager *crypto = CryptoManager_Create();
    
    char pwd[33];
    CryptoManager_GeneratePassword(crypto, pwd);
    printf("Senha: %s\n", pwd);
    
    CryptoManager_Destroy(crypto);
    return 0;
}
```

**Compilar:**
```bash
gcc -o app main.c crypto.c agle.c -lssl -lcrypto -O3
```

---

### Exemplo 3: Multi-arquivo com Headers

**arquivo: utils/security.h**
```c
#ifndef UTILS_SECURITY_H
#define UTILS_SECURITY_H

#include <stdint.h>

/* Gerar hash de string */
char* hash_password(const char *password);

/* Gerar token de sessão */
char* generate_session_token(void);

#endif
```

**arquivo: utils/security.c**
```c
#include "security.h"
#include "agle.h"
#include <stdlib.h>
#include <string.h>

static AGLE_CTX global_ctx = {0};
static int initialized = 0;

void _init_security(void) {
    if (!initialized) {
        AGLE_Init(&global_ctx);
        initialized = 1;
    }
}

char* hash_password(const char *password) {
    _init_security();
    
    char *hash_hex = malloc(65);
    uint8_t hash[32];
    
    AGLE_HashString(password, hash, 32);
    AGLE_BytesToHex(hash, 32, hash_hex);
    
    return hash_hex;  /* Caller deve free() */
}

char* generate_session_token(void) {
    _init_security();
    
    char *token = malloc(65);
    AGLE_GenerateSessionTokenHex(&global_ctx, token, 32);
    return token;  /* Caller deve free() */
}
```

**arquivo: main.c**
```c
#include "utils/security.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char *hash = hash_password("minha_senha");
    printf("Hash: %s\n", hash);
    
    char *token = generate_session_token();
    printf("Token: %s\n", token);
    
    free(hash);
    free(token);
    
    return 0;
}
```

---

## 🐛 Troubleshooting {#troubleshooting}

### Erro: "openssl/evp.h not found"

**Solução:**
```bash
# Ubuntu/Debian
sudo apt-get install libssl-dev

# CentOS/RHEL
sudo yum install openssl-devel

# macOS
brew install openssl
```

### Erro: "undefined reference to 'AGLE_Init'"

**Solução:**
```bash
# Verifique se agle.c está sendo compilado
gcc -o app main.c agle.c -lssl -lcrypto
#                ^^^^^^^ NECESSÁRIO!

# OU use a biblioteca
gcc -o app main.c -L./libs -lagle -lssl -lcrypto
#                   ^^^^^^^^^^^^^^^^^ LIBRARY PATH
```

### Erro de Linker: "-lssl" ou "-lcrypto" not found

**Solução:**
```bash
# Verificar instalação OpenSSL
pkg-config --cflags openssl
pkg-config --libs openssl

# Compilar com paths explícitos
gcc -o app main.c agle.c \
    $(pkg-config --cflags openssl) \
    $(pkg-config --libs openssl)
```

### Segmentation Fault

**Checklist:**
- [ ] Chamou `AGLE_Init()` antes de usar funções?
- [ ] Buffers têm tamanho suficiente?
- [ ] Não tá acessando memória liberada após `AGLE_Cleanup()`?

**Debug:**
```bash
valgrind ./app
# Mostra problemas de memória
```

---

## ✅ Checklist de Segurança {#checklist}

#### ✔️ Inicialização
- [ ] `AGLE_Init()` chamado no início
- [ ] Verificar retorno (é bool!)
- [ ] `AGLE_Cleanup()` chamado ao final

#### ✔️ Senhas
- [ ] Mínimo 16 caracteres
- [ ] Usar `AGLE_CHARSET_ALL` para máxima segurança
- [ ] Nunca armazenar em texto plano

#### ✔️ Hashing
- [ ] Sempre hashar senhas antes de armazenar
- [ ] Usar output mínimo 32 bytes (256 bits)
- [ ] Usar SHAKE256, não MD5 ou SHA1!

#### ✔️ KDF
- [ ] Salt único para cada usuário (16+ bytes)
- [ ] Iterações mínimo 100.000
- [ ] Para crítico: 1.000.000+ iterações

#### ✔️ Tokens
- [ ] Tokens com 32+ bytes
- [ ] Regenerar a cada sessão
- [ ] Nunca hardcode tokens!

#### ✔️ Limpeza de Memória
- [ ] Usar `AGLE_SecureZero()` para dados sensíveis
- [ ] Nunca deixar senhas em memória após uso
- [ ] Limpar buffers de salt/key

#### ✔️ Dependências
- [ ] OpenSSL 3.0+ instalado
- [ ] `-lssl -lcrypto` linked
- [ ] System com `/dev/urandom`

---

## 📚 Referência Rápida

```c
// Inicializar
AGLE_CTX ctx;
AGLE_Init(&ctx);

// Gerar senha
char pwd[33];
AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, pwd);

// Gerar token
char token[65];
AGLE_GenerateSessionTokenHex(&ctx, token, 32);

// Hash de string
char hash[65];
AGLE_HashSHAKE256_Hex("text", 4, hash, 32);

// Derivar chave
uint8_t salt[16], key[32];
AGLE_GetRandomBytes(&ctx, salt, 16);
AGLE_DeriveKey(pwd, strlen(pwd), salt, 16, 100000, key, 32);

// Limpeza
AGLE_SecureZero(pwd, sizeof(pwd));
AGLE_SecureZero(key, sizeof(key));
AGLE_Cleanup(&ctx);
```

---

## 🚀 Próximos Passos

1. **Copiar arquivos:** `cp agle.h agle.c seu_projeto/`
2. **Adaptar Makefile:** Incluir agle.c na compilação
3. **Incluir header:** `#include "agle.h"` nos seus arquivos
4. **Testar:** Compilar e rodar exemplos
5. **Integrar:** Usar funções AGLE em sua app

---

**Aproveite a flexibilidade da AGLE Library!** 🎯
