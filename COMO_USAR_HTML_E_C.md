# 🔐 Gerador & Validador de Senhas com AGLE

## 📌 O que você tem?

### 1. **senha_segura.html** - Página Web
- Interface bonita para gerar senhas
- **Mascarada por padrão** (mostra como `·····`)
- Mostra o Hash para armazenar no BD
- Gera Tokens de verificação
- 100% local (não envia dados)

### 2. **password_validator.c** - Programa C
- Valida senhas com AGLE
- Registra usuários com criptografia
- Protege contra força bruta (3 tentativas)
- Bloqueia conta por 5 minutos após 3 erros
- Armazena salt + hash no servidor

---

## 🚀 Como Usar?

### Opção 1: Apenas HTML (Demonstração)

```bash
# Abrir no navegador
firefox senha_segura.html

# Ou
google-chrome senha_segura.html
```

**O que faz:**
- ✅ Gera senhas aleatórias
- ✅ Mostra hash para banco de dados
- ✅ Mascarada na tela (tipo `M**h@S3n*a`)
- ❌ Não valida (é só gerador)

---

### Opção 2: Compilar & Rodar o Validador C (Produção)

#### **Passo 1: Compilar**

```bash
# Ir para o diretório
cd /home/andr/Documentos/Hardware-Induced-Irreversibility-in-Chaotic-Maps-main/

# Compilar
gcc -o password_validator password_validator.c agle.c -lssl -lcrypto -Wall -Wextra -std=c99

# Ou com otimização
gcc -O3 -march=native -o password_validator password_validator.c agle.c -lssl -lcrypto
```

#### **Passo 2: Rodar**

```bash
./password_validator
```

#### **Passo 3: Seguir o Menu**

```
╔═════════════════════════════════════════╗
║   Sistema de Validação de Senhas AGLE  ║
║                                         ║
║  Usando Criptografia com SHAKE256      ║
║  KDF: 100.000 iterações por senha      ║
╚═════════════════════════════════════════╝

1. Registrar novo usuário
2. Fazer login
3. Listar usuários
4. Sair

Escolha: _
```

---

## 📝 Exemplo Prático

### Registrar Usuário:

```
Escolha: 1

📝 REGISTRAR NOVO USUÁRIO
┌─────────────────────────────┐
Usuário: alice
Senha (mínimo 8 caracteres): MinhaSenh@123
└─────────────────────────────┘

✅ Usuário Registrado!
├─ Usuário: alice
├─ Senha: M***a3 (mascarada para exibição)
└─ Salt armazenado: a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6
```

### Fazer Login (Correto):

```
Escolha: 2

🔓 FAZER LOGIN
┌─────────────────────────────┐
Usuário: alice
Senha: MinhaSenh@123
└─────────────────────────────┘

✅ AUTENTICAÇÃO SUCESSO!
├─ Benvindo, alice!
├─ Tentativas (resetadas): 0/3
└─ Status: Desbloqueado
```

### Fazer Login (Errado - 3 tentativas):

```
Tentativa 1:
❌ SENHA INCORRETA!
├─ Tentativas: 1/3
└─ Cuidado! Mais 2 tentativas e conta bloqueia.

Tentativa 2:
❌ SENHA INCORRETA!
├─ Tentativas: 2/3
└─ Cuidado! Mais 1 tentativa e conta bloqueia.

Tentativa 3:
❌ SENHA INCORRETA!
├─ Tentativas: 3/3
└─ ⚠️ Conta BLOQUEADA por 5 minutos!

(Tente fazer login agora → conta bloqueada)
```

### Listar Usuários:

```
Escolha: 3

📋 USUÁRIOS REGISTRADOS:
┌──────────────────┬──────────────────┬──────────────┐
│ Usuário          │ Tentativas       │ Status       │
├──────────────────┼──────────────────┼──────────────┤
│ alice            │ 0/3              │ ✅ Ativo     │
│ bob              │ 2/3              │ ✅ Ativo     │
│ charlie          │ 3/3              │ 🔒 Bloqueado │
└──────────────────┴──────────────────┴──────────────┘
```

---

## 🔒 Como Funciona a Segurança?

### 1. Registração (Primeira Vez)

```c
┌──────────────────────────────────────────┐
│ Usuário digita:  "MinhaSenh@123"         │
└──────────────────────────────────────────┘
              ↓
      ┌───────────────────┐
      │ Gerar SALT        │ ← 16 bytes aleatórios
      │ aleatório         │
      └───────────────────┘
              ↓
      ┌───────────────────────────────────┐
      │ AGLE_DeriveKey(                   │
      │   senha,                          │
      │   salt,                           │
      │   100.000 iterações ← LENTO! 💨  │
      │ )                                 │
      └───────────────────────────────────┘
              ↓
      ┌──────────────────┐
      │ HASH (32 bytes)  │ ← Armazenar no BD
      │ no banco dados   │
      └──────────────────┘
```

### 2. Login (Validar Senha)

```c
┌──────────────────────────────────────────┐
│ Usuário digita:  "MinhaSenh@123"         │
└──────────────────────────────────────────┘
              ↓
      ┌────────────────────────────────────┐
      │ Buscar SALT armazenado no BD       │
      └────────────────────────────────────┘
              ↓
      ┌───────────────────────────────────┐
      │ AGLE_DeriveKey(                   │
      │   senha_digitada,                 │
      │   salt_do_BD,                     │
      │   100.000 iterações               │
      │ )                                 │
      └───────────────────────────────────┘
              ↓ Novo HASH
      ┌────────────────────────────────────┐
      │ Comparar:                          │
      │ novo_hash == hash_armazenado?      │
      └────────────────────────────────────┘
              ↓
         SIM  ou  NÃO
         ✅      ❌
```

### 3. Por que é Seguro?

```
🔐 Salt Aleatório
   └─ Cada usuário tem salt único
   └─ Impede ataques Rainbow Table

⏱️ 100.000 Iterações (LENTO!)
   └─ Demora ~1 segundo para legitimar
   └─ Demora horas para força bruta
   └─ Protege contra GPU attacks

🔒 SHAKE256 (SHA-3)
   └─ Algoritmo criptográfico moderno
   └─ Resistente a colisões
   └─ NIST certificado

🚫 Proteção Força Bruta
   └─ Máximo 3 tentativas
   └─ Bloqueia 5 minutos
   └─ Impossível password spray
```

---

## 💻 Compilações Diferentes

### Compilação Simples (Debug)
```bash
gcc -o password_validator password_validator.c agle.c -lssl -lcrypto
```

### Compilação Otimizada (Produção)
```bash
gcc -O3 -march=native -o password_validator password_validator.c agle.c -lssl -lcrypto
```

### Compilação com Debug Symbols (para gdb)
```bash
gcc -g -o password_validator password_validator.c agle.c -lssl -lcrypto
gdb ./password_validator
```

---

## 🌐 Integração Web (Futuro)

Se quiser conectar HTML + C:

### 1. Criar API REST em C

```c
// Usar bibliotecas como:
// - libmicrohttpd (HTTP server)
// - libcurl (client}
// - cJSON (JSON parsing)
```

### 2. JavaScript no HTML

```javascript
// Enviar para backend C:
fetch('http://localhost:8080/login', {
    method: 'POST',
    body: JSON.stringify({
        username: 'alice',
        password: 'MinhaSenh@123'
    })
});
```

### 3. C recebe e valida

```c
// password_validator_api.c (não criado ainda)
// Rodaria em localhost:8080
// Validaria senha com AGLE
// Retornaria JSON
```

---

## 📊 Comparação: HTML vs C

```
ASPECTO              │ HTML         │ Programa C
─────────────────────┼──────────────┼──────────────
Gera senhas?         │ ✅ Sim       │ ✅ Sim
Mascarada na tela?   │ ✅ Sim       │ ✅ Sim (com *)
Valida senhas?       │ ❌ Não       │ ✅ Sim
Armazena usuários?   │ ❌ Não       │ ✅ Sim (RAM)
Protege força bruta? │ ❌ Não       │ ✅ Sim (bloqueia)
Criptografia AGLE?   │ ❌ Simulada  │ ✅ Verdadeira
Segurança KDF?       │ ❌ Não       │ ✅ 100k iterações
Pronto produção?     │ ❌ Demo      │ ⚠️  Aprox (usar BD)
```

---

## ✅ Checklist: Começar Agora

- [ ] Abrir `senha_segura.html` no navegador
- [ ] Gerar algumas senhas (vê como mascarada)
- [ ] Compilar `password_validator.c`
- [ ] Rodar programa
- [ ] Registrar usuário alice/MinhaSenh@123
- [ ] Fazer login (sucesso)
- [ ] Tentar login 3x com senha errada
- [ ] Ver conta bloqueada
- [ ] Esperar 5 minutos (ou editar código)
- [ ] Fazer login novamente (sucesso)

---

## 🎯 Próximos Passos Para Produção

1. **Armazenar usuários em banco**
   ```c
   // Trocar UserRecord[] por SQLite/PostgreSQL
   // SQL: INSERT INTO users VALUES (username, salt, hash)
   ```

2. **API REST**
   ```c
   // Usar libmicrohttpd para HTTP
   // Endpoints: /register, /login, /logout
   ```

3. **HTTPS**
   ```c
   // Usar OpenSSL para SSL/TLS
   // Certificado válido em produção
   ```

4. **Sessions**
   ```c
   // Gerar token de sessão após login
   // Armazenar em cookies/localStorage
   ```

5. **2FA (Autenticação de Dois Fatores)**
   ```c
   // Adicionar OTP/SMS/Email verification
   // AGLE_GenerateNonce() para código temporário
   ```

---

## 📞 Precisa de Ajuda?

| Problema | Solução |
|----------|---------|
| HTML não abre | Usar navegador moderno (Chrome, Firefox) |
| C não compila | Instalar OpenSSL: `sudo apt-get install libssl-dev` |
| Senha muito curta | Mínimo 8 caracteres (editar no código) |
| Programa travou | Esperando 5 min de bloqueio? Ctrl+C restart |
| Qual função AGLE? | Ver FUNCOES.md ou LIBRARY.md |

---

## 🔐 Lembrete de Segurança

⚠️ **PARA PRODUÇÃO, VOCÊ PRECISA:**

- ✅ Banco de dados (SQLite, PostgreSQL, MySQL)
- ✅ HTTPS/SSL (não HTTP normal!)
- ✅ Rate limiting (além dos 3 logins)
- ✅ CAPTCHA (previne bots)
- ✅ Logs de auditoria (quem tentou logar)
- ✅ Backup de dados
- ✅ Monitoramento de segurança
- ✅ Atualização frequente de OpenSSL

**Este código é educacional. Não use diretamente em produção sem hardening!**

---

**Status:** ✅ Pronto para Usar
**Versão:** AGLE v2.0.0
**Data:** 9 de Fevereiro de 2026
