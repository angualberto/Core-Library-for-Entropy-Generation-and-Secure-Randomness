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
