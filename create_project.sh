#!/bin/bash
# Template criador para novos projetos com AGLE
# Uso: bash create_project.sh meu_projeto

if [ -z "$1" ]; then
    echo "Uso: bash create_project.sh <nome_do_projeto>"
    echo "Exemplo: bash create_project.sh meu_app_seguro"
    exit 1
fi

PROJECT_NAME=$1
mkdir -p "$PROJECT_NAME"
cd "$PROJECT_NAME"

echo "✅ Criando estrutura para: $PROJECT_NAME"

# Copiar arquivos da biblioteca
echo "📋 Copiando AGLE library..."
cp ../agle.h .
cp ../agle.c .
cp ../Makefile.lib Makefile.lib

# Criar arquivo main
cat > main.c << 'EOF'
#include "agle.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    AGLE_CTX ctx;
    
    // Inicializar
    if (!AGLE_Init(&ctx)) {
        fprintf(stderr, "Erro ao inicializar AGLE\n");
        return 1;
    }
    
    // EXEMPLO: Gerar uma senha
    char password[33];
    if (AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, password)) {
        printf("✅ Senha gerada: %s\n", password);
        AGLE_SecureZero(password, sizeof(password));
    }
    
    // EXEMPLO: Gerar um token (32 bytes = 64 chars hex)
    char token[65];
    if (AGLE_GenerateSessionTokenHex(&ctx, token, 32)) {
        printf("✅ Token: %s\n", token);
    }
    
    // EXEMPLO: Hash com SHAKE256 (hexadecimal)
    char hash[65];
    if (AGLE_HashSHAKE256_Hex((uint8_t*)"dados", 5, hash, 32)) {
        printf("✅ Hash: %s\n", hash);
    }
    
    // EXEMPLO: números aleatórios
    uint32_t numero;
    if (AGLE_GetRandomInt(&ctx, 100, &numero)) {
        printf("✅ Número aleatório (0-99): %u\n", numero);
    }
    
    // Limpar
    AGLE_Cleanup(&ctx);
    return 0;
}
EOF

# Criar Makefile customizado
cat > Makefile << 'EOF'
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O3 -march=native
LIBS = -lssl -lcrypto

# Detectar se já tem a library instalada
AGLE_LIB = $(shell pkg-config --cflags --libs libssl libcrypto 2>/dev/null || echo "-lssl -lcrypto")

TARGET = meu_app
SOURCES = main.c agle.c
OBJECTS = $(SOURCES:.c=.o)
HEADER = agle.h

# Compile options
ifdef DEBUG
    CFLAGS = -Wall -Wextra -std=c99 -g -O0 -DDEBUG
endif

ifdef RELEASE
    CFLAGS = -Wall -Wextra -std=c99 -O3 -march=native -DNDEBUG
endif

.PHONY: all clean run help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "🔗 Ligando: $@"
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "✅ Compilado: ./$@"

%.o: %.c $(HEADER)
	@echo "📦 Compilando: $<"
	$(CC) $(CFLAGS) -c $<

run: $(TARGET)
	@echo "🚀 Executando .../$(TARGET)..."
	./$(TARGET)

debug:
	@echo "🐛 Compilando com debug..."
	$(MAKE) DEBUG=1 clean all

release:
	@echo "⚡ Compilando versão release..."
	$(MAKE) RELEASE=1 clean all
	strip $(TARGET)

clean:
	@echo "🗑️  Limpando..."
	rm -f *.o $(TARGET)
	@echo "✅ Limpo"

help:
	@echo "Comandos disponíveis:"
	@echo "  make           - Compilar (padrão)"
	@echo "  make run       - Compilar e executar"
	@echo "  make debug     - Compilar com debug (-g)"
	@echo "  make release   - Versão otimizada"
	@echo "  make clean     - Remover binários"
	@echo "  make help      - Este menu"
EOF

# Criar README
cat > README.md << 'EOF'
# '"$PROJECT_NAME"'

Projeto usando AGLE Cryptographic Library

## Início Rápido

```bash
# Compilar
make

# Executar
make run

# Debug
make debug

# Release
make release
```

## Arquivos

- `main.c` - Seu código principal
- `agle.h` - Header da biblioteca AGLE
- `agle.c` - Implementação da AGLE
- `Makefile` - Build system

## Estrutura de Arquivos

```
.
├── main.c              # Seu código
├── agle.h              # API AGLE
├── agle.c              # Impl. AGLE
├── Makefile            # Build
├── README.md           # Este arquivo
└── meu_app             # Executável (após make)
```

## Funções Disponíveis

### Inicializar
```c
AGLE_CTX ctx;
AGLE_Init(&ctx);       // SEMPRE primeiro
AGLE_Cleanup(&ctx);    // SEMPRE último
```

### 🔑 Senhas
```c
char pwd[33];
AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, pwd);
```

### 🎲 Números Aleatórios
```c
uint32_t num = AGLE_GetRandomInt(&ctx, 100);     // 0-99
uint64_t big = AGLE_GetRandom64(&ctx);
uint8_t bytes[16];
AGLE_GetRandomBytes(&ctx, bytes, 16);
```

### #️⃣ Hash
```c
char hash[65];
AGLE_HashString("dados", 5, 256, hash);
```

### 🔐 KDF (Derivar chave)
```c
uint8_t salt[16], key[32];
AGLE_GetRandomBytes(&ctx, salt, 16);
AGLE_DeriveKey(&ctx, "senha", 5, salt, 16, 100000, key);
```

### 🎫 Tokens
```c
char token[65];
AGLE_GenerateSessionTokenHex(&ctx, token);

uint64_t nonce = AGLE_GenerateNonce(&ctx);
```

### 🧹 Limpar memória
```c
AGLE_SecureZero(pwd, sizeof(pwd));
```

## Requisitos

- GCC ou Clang
- OpenSSL 3.0+ (`sudo apt-get install libssl-dev`)
- Linux/Unix (para /dev/urandom)

## Segurança

✅ SEMPRE:
- Chamar `AGLE_Init()` no início
- Chamar `AGLE_Cleanup()` no final
- Usar `AGLE_SecureZero()` para dados sensíveis
- Usar salts aleatórios

❌ NUNCA:
- Hardcodear senhas
- Reutilizar salts
- Usar menos de 100k iterações na KDF

## Próximos Passos

1. Editar `main.c` para seu use case
2. `make run` para testar
3. `make release` para produção

## Documentação Completa

- `FUNCOES.md` - Todas as 14 funções
- `CHEAT_SHEET.md` - Referência rápida
- `LIBRARY.md` - Documentação completa
- `QUICK_START.md` - Começar do zero

---

Criado com ❤️ usando AGLE v2.0.0
EOF

# Criar arquivo .gitignore
cat > .gitignore << 'EOF'
# Binários
*.o
*.a
*.so
*.dll
*.exe
meu_app

# Debug
.gdb_history
*.dSYM/
*.su
*.gch

# IDE
.vscode/
.idea/
*.swp
*.swo
*~

# OS
.DS_Store
Thumbs.db
EOF

# Criar estrutura de diretórios
mkdir -p src include build/obj build/bin

echo ""
echo "📂 Estrutura do projeto:"
ls -la

echo ""
echo "✅ Projeto criado com sucesso!"
echo ""
echo "Próximos passos:"
echo "  1. cd $PROJECT_NAME"
echo "  2. make (compilar)"
echo "  3. make run (executar)"
echo "  4. Editar main.c para seu use case"
echo ""
echo "Documentação:"
echo "  • FUNCOES.md - Todas as funções"
echo "  • CHEAT_SHEET.md - Referência rápida"
echo "  • LIBRARY.md - Documentação completa"
