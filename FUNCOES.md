
# AGLE - Todas as Funções (Referência Completa)

## 📋 Índice Visual

```
┌─────────────────────────────────────────┐
│ CATEGORIA                  │ FUNÇÕES    │
├─────────────────────────────────────────┤
│ 1. Init/Cleanup            │ 2 funções  │
│ 2. Números Aleatórios      │ 3 funções  │
│ 3. Senhas                  │ 2 funções  │
│ 4. Hashing                 │ 2 funções  │
│ 5. KDF (Derivação)         │ 1 função   │
│ 6. Tokens/Nonces           │ 2 funções  │
│ 7. Utilidades              │ 2 funções  │
└─────────────────────────────────────────┘
Total: 14 funções prontas para usar
```

---

## 1️⃣ INICIALIZAR & LIMPAR

```c
bool AGLE_Init(AGLE_CTX *ctx)
├─ Inicializa o contexto
├─ Abre /dev/urandom
├─ Carrega 4KB de entropia
└─ Retorna: true = sucesso, false = erro

bool AGLE_Cleanup(AGLE_CTX *ctx)
├─ Fecha /dev/urandom
├─ Limpa memória
├─ Finaliza contexto
└─ Retorna: true = OK, false = erro
```

### Como usar:
```c
AGLE_CTX ctx;

// SEMPRE comecar com Init!
if (!AGLE_Init(&ctx)) {
    printf("Erro ao inicializar\n");
    return 1;
}

// ... fazer coisas ...

// SEMPRE terminar com Cleanup!
AGLE_Cleanup(&ctx);
```

---

## 2️⃣ NÚMEROS ALEATÓRIOS

```c
bool AGLE_GetRandomBytes(AGLE_CTX *ctx, uint8_t *buffer, size_t size)
├─ Gera 'size' bytes aleatórios
├─ Muito rápido (<1ms para 1KB)
└─ Usa: /dev/urandom + SHAKE256
│
├─ Exemplo: Gerar 16 bytes
│  uint8_t bytes[16];
│  AGLE_GetRandomBytes(&ctx, bytes, 16);
│
└─ Uso: Chaves, salts, tokens

uint32_t AGLE_GetRandomInt(AGLE_CTX *ctx, uint32_t max)
├─ Retorna número 0 até (max-1)
├─ Muito rápido (<1ms)
└─ Exemplo: 
   uint32_t numero = AGLE_GetRandomInt(&ctx, 100);  // 0-99
   
uint64_t AGLE_GetRandom64(AGLE_CTX *ctx)
├─ Retorna número de 0 a 2^64-1
├─ Muito rápido
└─ Exemplo:
   uint64_t grande = AGLE_GetRandom64(&ctx);
```

### Tabela de Performance:
```
Operação                 Tempo típico
────────────────────────────────────
GetRandomBytes(16)       ~0.1ms
GetRandomBytes(1KB)      ~1ms
GetRandomInt(100)        ~0.1ms
GetRandom64()            ~0.1ms
```

---

## 3️⃣ GERAÇÃO DE SENHAS

```c
bool AGLE_GeneratePassword(AGLE_CTX *ctx, AGLE_CharsetFlags charset, 
                            size_t length, char *output)
├─ Gera senha aleatória
├─ Charset (flags combinávels):
│  ├─ AGLE_CHARSET_LOWERCASE    (a-z)
│  ├─ AGLE_CHARSET_UPPERCASE    (A-Z)
│  ├─ AGLE_CHARSET_DIGITS       (0-9)
│  ├─ AGLE_CHARSET_SYMBOLS      (!@#$...)
│  └─ AGLE_CHARSET_ALL          (todos)
├─ Length: 4 a 256 caracteres
└─ output: buffer >= length+1

Exemplo 1: Senha super forte
  char pwd[33];
  AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, pwd);
  // pwd = ")BJ29FdZ2J$uRQMo0E]T$wD)Y8/Akz6L"

Exemplo 2: Senha alfanumérica (ID de usuário)
  char id[17];
  AGLE_GeneratePassword(&ctx, 
    AGLE_CHARSET_UPPERCASE | AGLE_CHARSET_DIGITS, 16, id);
  // id = "A7B2C9D1E4F6G8H3"

Exemplo 3: Apenas letras (menos caracteres especiais)
  char pwd[25];
  AGLE_GeneratePassword(&ctx, 
    AGLE_CHARSET_UPPERCASE | AGLE_CHARSET_LOWERCASE, 24, pwd);
  // pwd = "eXaMpLeOfAaAaAaAaAaAaAa"

bool AGLE_GeneratePassphrase(AGLE_CTX *ctx, size_t word_count,
                               const char *separator, char *output, 
                               size_t output_size)
├─ Gera frase com palavras aleatórias
├─ Melhor para senhas memoráveis
├─ Tem ~60 palavras no dicionário
├─ Usa separador (ex: "-" ou "_")
└─ Mais seguro que parece!

Exemplo 1: Frase de 5 palavras com hífen
  char frase[256];
  AGLE_GeneratePassphrase(&ctx, 5, "-", frase, sizeof(frase));
  // frase = "desert-cipher-moon-rho-desert"

Exemplo 2: Frase de 8 palavras com underscore
  char frase[256];
  AGLE_GeneratePassphrase(&ctx, 8, "_", frase, sizeof(frase));
  // frase = "alpha_beta_gamma_delta_epsilon_zeta_eta_theta"

Combinações possíveis: 60^5 = 777 milhões (5 palavras)
                      60^8 = 1.6 trilhões (8 palavras)
```

### Comparação:
```
Tipo                    Memorável?   Forte?   Uso
─────────────────────────────────────────────────
Senha aleatória (32)    ❌ Ruim      ✅ Ótimo  BD, API
Passphrase (5 palavras) ✅ Ótimo     ✅ Ótimo  Usuários humanos
Passphrase (8 palavras) ✅ Ótimo     ✅✅ Máx   Senhas críticas
```

---

## 4️⃣ HASHING (SHAKE256)

```c
bool AGLE_HashSHAKE256(const uint8_t *data, size_t data_len,
                        size_t output_bits, char *hex_output)
├─ Faz hash com SHAKE256 (SHA-3)
├─ output_bits: 256 ou 512 (ou outro múltiplo de 8)
├─ hex_output: string em hexadecimal
└─ Exemplo: 32 bytes input → 64 chars hex (256-bit)

Exemplo 1: Hash 256-bit de uma senha
  char hash[65];  // 64 chars + null
  AGLE_HashSHAKE256((uint8_t*)"senha123", 8, 256, hash);
  // hash = "8896d798e25c2a4bd7768b5398ed3fa768b3535"

Exemplo 2: Hash 512-bit (mais forte!)
  char hash[129];  // 128 chars + null
  AGLE_HashSHAKE256((uint8_t*)"dados", 5, 512, hash);
  // hash = "a2b3c4d5e6f7..."

bool AGLE_HashString(const char *str, size_t str_len,
                      size_t output_bits, char *hex_output)
├─ Mesmo que AGLE_HashSHAKE256 mas recebe char*
├─ Mais conveniente para strings
└─ Exemplo:
   char hash[65];
   AGLE_HashString("password123", 12, 256, hash);
```

### Casos de Uso:
```
Uso                    Hash Size   Exemplo
────────────────────────────────────────────────
Password hashing       256-bit     Banco de dados
File checksum          256-bit     Integridade
Security token         512-bit     Máxima segurança
Database key           256-bit     Índices
```

---

## 5️⃣ DERIVAÇÃO DE CHAVE (KDF)

```c
bool AGLE_DeriveKey(AGLE_CTX *ctx, const char *password, size_t pwd_len,
                     const uint8_t *salt, size_t salt_len,
                     uint32_t iterations, uint8_t *derived_key)
├─ Deriva chave criptográfica de senha
├─ Protege contra ataques (força bruta)
├─ iterations: quanto maior = mais tempo
├─ Recomendado: 100,000 iterações (≈1 segundo)
└─ Produz: 32 bytes de chave

⚠️ LENTO DE PROPÓSITO! Isto é SEGURANÇA!
   100k iterações = ~1 segundo (é normal)
   1M iterações = ~10 segundos (para casos críticos)

Exemplo Completo: Autenticação
  
  // FASE 1: Usuário faz cadastro
  uint8_t salt[16];
  uint8_t hash[32];
  AGLE_GetRandomBytes(&ctx, salt, 16);  // Salt aleatório
  AGLE_DeriveKey(&ctx, "minha_senha", 11, salt, 16, 
                 100000, hash);
  // Salvar no BD: salt (16 bytes) + hash (32 bytes)
  
  // FASE 2: Usuário faz login
  uint8_t hash_tentativa[32];
  AGLE_DeriveKey(&ctx, "minha_senha", 11, salt_do_BD, 16,
                 100000, hash_tentativa);
  
  // Comparar
  if (memcmp(hash, hash_tentativa, 32) == 0) {
      printf("✅ Senha correta!\n");
  } else {
      printf("❌ Senha errada\n");
  }
```

### Recomendações:
```
Contexto              Iterações    Tempo     Segurança
────────────────────────────────────────────────────
Usuário normal        100,000      1s        ✅ Bom
Admin/crítico         1,000,000    10s       ✅✅ Ótimo
Muito crítico         10,000,000   100s      ✅✅✅ Máx
(Não recomendado)
```

---

## 6️⃣ TOKENS & NONCES

```c
bool AGLE_GenerateSessionToken(AGLE_CTX *ctx, uint8_t *output)
├─ Gera token de sessão (32 bytes binários)
├─ Aleatório criptograficamente
└─ Bom para: headers da API, cookies

Exemplo:
  uint8_t token[32];
  AGLE_GenerateSessionToken(&ctx, token);
  // token = {0x54, 0xaa, 0xd6, ...}

bool AGLE_GenerateSessionTokenHex(AGLE_CTX *ctx, char *hex_output)
├─ Mesmo que acima, mas no formato hexadecimal
├─ hex_output = 64 chars + null terminator
└─ Mais conveniente para APIs/web

Exemplo:
  char token[65];
  AGLE_GenerateSessionTokenHex(&ctx, token);
  // token = "54aad6619000b5a557c27f686e512163102f0c6a73ff..."

uint64_t AGLE_GenerateNonce(AGLE_CTX *ctx)
├─ Gera "número usado uma só vez"
├─ Para protocolos/rate limiting
├─ Cada chamada = valor diferente
└─ Exemplo:
   uint64_t nonce = AGLE_GenerateNonce(&ctx);
   // nonce = 13954545744792224786

Exemplo Real: API com Rate Limiting
  char api_token[65];
  uint64_t nonce;
  
  for (int i = 0; i < 3; i++) {
      AGLE_GenerateSessionTokenHex(&ctx, api_token);
      nonce = AGLE_GenerateNonce(&ctx);
      printf("Request %d: Token=%s, Nonce=%llu\n", 
             i, api_token, nonce);
  }
```

---

## 7️⃣ UTILIDADES

```c
void AGLE_SecureZero(void *data, size_t len)
├─ Limpa memória com segurança
├─ Impede otimização do compilador
├─ CRÍTICO para senhas/chaves!
└─ Exemplo:
   char senha[33] = "MinhaSenh@123";
   AGLE_SecureZero(senha, sizeof(senha));
   // senha agora é todo zeros

const char* AGLE_ErrorMessage(int error_code)
├─ Retorna mensagem de erro em português
├─ error_code: número do erro
└─ Exemplo:
   if (!AGLE_Init(&ctx)) {
       printf("Erro: %s\n", AGLE_ErrorMessage(-1));
   }

bool AGLE_BytesToHex(const uint8_t *bytes, size_t len,
                      char *hex_output)
├─ Converte bytes para hexadecimal
├─ hex_output = len*2 caracteres + null
└─ Exemplo:
   uint8_t data[4] = {0x12, 0x34, 0x56, 0x78};
   char hex[9];
   AGLE_BytesToHex(data, 4, hex);
   // hex = "12345678"
```

---

## 🎯 Flowchart: Como Usar?

```
                    START
                      |
                      ↓
             ┌─────────────────┐
             │ AGLE_Init(&ctx) │
             └─────────────────┘
                      |
         ┌────────────┼────────────┐
         |            |            |
         ↓            ↓            ↓
    Gerar Senha   Gerar Token  Fazer Hash
    Hash/KDF      Números Aleat. Verificação
         |            |            |
         └────────────┼────────────┘
                      |
                      ↓
               ┌─────────────────────┐
               │ AGLE_SecureZero()   │
               │ (se for sensível)   │
               └─────────────────────┘
                      |
                      ↓
             ┌─────────────────────┐
             │ AGLE_Cleanup(&ctx)  │
             └─────────────────────┘
                      |
                      ↓
                     END & SUCCESS ✅
```

---

## 📊 Tabela Rápida: Qual Função Usar?

```
NECESSIDADE                    FUNÇÃO              OUTPUT
─────────────────────────────────────────────────────────
Número aleatório simples       GetRandomInt        uint32_t
Número grande aleatório        GetRandom64         uint64_t
Bytes aleatórios (16+)         GetRandomBytes      bytes[]
Senha para usuário             GeneratePassword    string 32ch
Senha memorável                GeneratePassphrase  "word-word"
Hash de dados                  HashSHAKE256        hex string
Derivar chave de senha         DeriveKey           bytes[]
Token de sessão                GenTokenHex         hex string
Nonce único                    GenerateNonce       uint64_t
Limpar variável sensível       SecureZero          (nada)
```

---

## ✅ Checklist Para Usar AGLE

- [ ] Incluir `#include "agle.h"`
- [ ] Criar variável `AGLE_CTX ctx`
- [ ] Chamar `AGLE_Init(&ctx)` NO INÍCIO
- [ ] Usar as funções necessárias
- [ ] Se usar senhas: chamar `AGLE_SecureZero()`
- [ ] Chamar `AGLE_Cleanup(&ctx)` NO FINAL
- [ ] Compilar com `-lagle -lssl -lcrypto`
- [ ] Testar com valores pequenos primeiro ✓
- [ ] Aumentar tamanhos gradualmente ✓
- [ ] Deployar com confiança! 🚀

---

## 🚀 Template Mínimo (Copy-Paste)

```c
#include "agle.h"
#include <stdio.h>
#include <string.h>

int main() {
    AGLE_CTX ctx;
    
    // Inicializar
    if (!AGLE_Init(&ctx)) {
        printf("Erro ao inicializar AGLE\n");
        return 1;
    }
    
    // TODO: Usar as funções AGLE_*
    
    // Limpar
    AGLE_Cleanup(&ctx);
    return 0;
}

// Compilar:
// gcc -o programa programa.c -lagle -lssl -lcrypto
```

---

**Criado:** 9 de Fevereiro de 2026
**Versão:** AGLE v2.0.0
**Total de Funções:** 14 prontas para usar
