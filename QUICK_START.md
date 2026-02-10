# AGLE Library - Quick Start Guide

## Iniciando rapidamente

```bash
cmake -S . -B build
cmake --build build
./build/agle_examples
```

## Estrutura de arquivos

```
include/         # header publico (API)
src/             # implementacao da biblioteca
examples/        # exemplos completos
LIBRARY.md       # documentacao completa
Makefile         # build alternativo
build/           # artefatos de compilacao
```

## Usar a biblioteca no seu codigo

### Opcao 1: CMake (recomendado)

```bash
cmake -S . -B build
cmake --build build
sudo cmake --install build
```

### Opcao 2: Makefile

```bash
make all
sudo make install
```

### Opcao 3: Compilacao manual

```bash
gcc -Iinclude -o seu_app seu_app.c src/agle.c -lssl -lcrypto -O3
```

## Exemplos rapidos

### Gerar senha segura

```c
#include "agle.h"
#include <stdio.h>

int main(void) {
    AGLE_CTX ctx;
    AGLE_Init(&ctx);

    char password[33];
    AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, password);
    printf("Senha: %s\n", password);

    AGLE_Cleanup(&ctx);
    return 0;
}
```

### Gerar token de API

```c
#include "agle.h"
#include <stdio.h>

int main(void) {
    AGLE_CTX ctx;
    AGLE_Init(&ctx);

    char api_token[65];
    AGLE_GenerateSessionTokenHex(&ctx, api_token, 32);
    printf("API Key: %s\n", api_token);

    AGLE_Cleanup(&ctx);
    return 0;
}
```

## Comandos Make uteis

```bash
make help              # lista comandos disponiveis
make all               # compila tudo
make static            # static library
make shared            # shared library
make examples          # exemplos
make run               # roda exemplos
make clean             # limpa build/
make install           # instala em /usr/local
```

## Documentacao

- [LIBRARY.md](LIBRARY.md) - documentacao completa da API
- [README.md](README.md) - visao geral do projeto
- examples/agle_examples.c - exemplos completos
- examples/agle_quickstart.c - exemplo rapido

## Requisitos

- Compilador C99 (GCC ou Clang)
- OpenSSL 3.0+
- Linux (principal)

## Solucao de problemas

### libssl.so not found

```bash
sudo apt-get install libssl-dev
```

### Segmentation fault

- Verificar se AGLE_Init() foi chamado antes de usar
- Usar valgrind para debug: valgrind ./seu_app

```c
#include "agle.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    /* 1. Inicializar */
    AGLE_CTX ctx;
    if (!AGLE_Init(&ctx)) return 1;
    
    /* 2. Gerar senha */
    char pwd[33];
    AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, pwd);
    printf("Senha: %s\n", pwd);
    
    /* 3. Hash da senha */
    char hash[65];
    AGLE_HashSHAKE256_Hex((uint8_t*)pwd, strlen(pwd), hash, 32);
    printf("Hash: %s\n", hash);
    
    /* 4. Token de sessão */
    char token[65];
    AGLE_GenerateSessionTokenHex(&ctx, token, 32);
    printf("Token: %s\n", token);
    
    /* 5. Limpar */
    AGLE_SecureZero(pwd, sizeof(pwd));
    AGLE_Cleanup(&ctx);
    
    return 0;
}
```

---

**Aproveite a Biblioteca AGLE!** 🚀
