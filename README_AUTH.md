# 🔐 SISTEMA DE AUTENTICAÇÃO SEGURA COM AGLE

Sistema completo de autenticação com **protocolo de comunicação seguro** usando a biblioteca AGLE.

## 📋 O QUE É ESTE SISTEMA?

Um servidor HTTP com API REST que fornece autenticação segura usando:
- **Criptografia AGLE** (SHAKE256 + KDF com 100.000 iterações)
- **Tokens de sessão** únicos e seguros
- **Salt único** por usuário
- **Protocolo HTTP/REST** para comunicação cliente-servidor
- **Interface web moderna** para interação

---

## 🚀 INÍCIO RÁPIDO (30 SEGUNDOS)

```bash
# 1. Dar permissão de execução
chmod +x iniciar_auth.sh

# 2. Iniciar tudo automaticamente
./iniciar_auth.sh
```

**Pronto!** O navegador abrirá automaticamente. 🎉

---

## 📡 ARQUITETURA DO SISTEMA

```
┌─────────────────┐         HTTP/REST         ┌──────────────────┐
│                 │  ◄──────────────────────►  │                  │
│  cliente_auth   │      JSON Protocol        │  servidor_auth   │
│     (HTML/JS)   │                           │      (C/AGLE)    │
│                 │  POST /register           │                  │
└─────────────────┘  POST /login              └──────────────────┘
                     GET  /validate                     │
                     POST /logout                       │
                     GET  /stats                        ▼
                                              ┌──────────────────┐
                                              │  Criptografia    │
                                              │  - SHAKE256      │
                                              │  - KDF 100k iter │
                                              │  - Salt único    │
                                              │  - Tokens 256bit │
                                              └──────────────────┘
```

---

## 🔌 PROTOCOLO DE COMUNICAÇÃO

### **1. Registro de Usuário**
```http
POST /register
Content-Type: application/json

{
  "username": "alice",
  "password": "MinhaSenh@Segura123"
}
```

**Resposta:**
```json
{
  "success": true,
  "message": "Usuário registrado!"
}
```

**Segurança:**
- Gera salt aleatório de 16 bytes
- Deriva hash com KDF (100.000 iterações)
- Armazena apenas hash (nunca a senha)

---

### **2. Login (Autenticação)**
```http
POST /login
Content-Type: application/json

{
  "username": "alice",
  "password": "MinhaSenh@Segura123"
}
```

**Resposta:**
```json
{
  "success": true,
  "token": "86f5c3dc77d6085a478e01ee1d10070ca5f7fad2b9f43c992482342c141b5914",
  "username": "alice"
}
```

**Segurança:**
- Valida senha usando KDF com salt armazenado
- Gera token de sessão único (256 bits)
- Token expira em 1 hora

---

### **3. Validação de Token**
```http
GET /validate
Authorization: Bearer 86f5c3dc77d6085a478e01ee1d10070ca5f7fad2b9f43c992482342c141b5914
```

**Resposta:**
```json
{
  "success": true,
  "username": "alice",
  "expires_in": 3420
}
```

**Uso:** Validar se token ainda está ativo e verificar tempo restante.

---

### **4. Logout**
```http
POST /logout
Content-Type: application/json

{
  "token": "86f5c3dc77d6085a478e01ee1d10070ca5f7fad2b9f43c992482342c141b5914"
}
```

**Resposta:**
```json
{
  "success": true,
  "message": "Logout realizado"
}
```

---

### **5. Estatísticas do Servidor**
```http
GET /stats
```

**Resposta:**
```json
{
  "users": 5,
  "sessions": 12,
  "active_sessions": 3
}
```

---

## 🔐 SEGURANÇA IMPLEMENTADA

| Técnica | Descrição |
|---------|-----------|
| **SHAKE256** | Hash criptográfico SHA-3 (256 bits) |
| **KDF** | Key Derivation Function com 100.000 iterações |
| **Salt único** | 16 bytes aleatórios por usuário |
| **Tokens 256-bit** | Gerados com AGLE_GenerateSessionTokenHex |
| **Expiração** | Sessões expiram em 1 hora |
| **CORS habilitado** | Para comunicação cross-origin |

---

## 💻 USO MANUAL

### **1. Compilar o Servidor**
```bash
gcc -O2 -o servidor_auth servidor_auth.c agle.c -lssl -lcrypto
```

### **2. Iniciar o Servidor**
```bash
./servidor_auth
```

Servidor estará em: `http://localhost:8080`

### **3. Abrir a Interface**
```bash
firefox cliente_auth.html
# ou
google-chrome cliente_auth.html
```

---

## 🧪 TESTAR COM CURL

### **Registrar usuário:**
```bash
curl -X POST http://localhost:8080/register \
  -H "Content-Type: application/json" \
  -d '{"username":"alice","password":"Senha123!@#"}'
```

### **Fazer login:**
```bash
curl -X POST http://localhost:8080/login \
  -H "Content-Type: application/json" \
  -d '{"username":"alice","password":"Senha123!@#"}'
```

### **Validar token:**
```bash
TOKEN="seu_token_aqui"
curl -X GET http://localhost:8080/validate \
  -H "Authorization: Bearer $TOKEN"
```

### **Ver estatísticas:**
```bash
curl http://localhost:8080/stats
```

---

## 📂 ARQUIVOS DO SISTEMA

```
servidor_auth.c       → Servidor HTTP com API REST (450 linhas)
cliente_auth.html     → Interface web moderna (350 linhas)
iniciar_auth.sh       → Script de inicialização automática
README_AUTH.md        → Este arquivo (documentação)
agle.c / agle.h       → Biblioteca de criptografia
```

---

## 🎯 CASOS DE USO

### **1. Aplicação Web Completa**
Substitua o HTML estático por sua aplicação (React, Vue, etc.) e use a API REST.

### **2. Aplicativo Mobile**
Use os mesmos endpoints REST para autenticar apps Android/iOS.

### **3. Microserviço de Autenticação**
Integre este servidor como microserviço em arquiteturas maiores.

### **4. Sistema Desktop**
Crie clientes em Python, Java, etc. que se comuniquem via HTTP.

---

## 🔧 PERSONALIZAÇÃO

### **Alterar Porta:**
Edite `servidor_auth.c` linha 13:
```c
#define PORT 8080  // Mude para sua porta
```

### **Tempo de Expiração:**
Edite `servidor_auth.c` linha 15:
```c
#define SESSION_TIMEOUT 3600  // Segundos (1 hora)
```

### **Iterações do KDF:**
Busque por `100000` no código e altere o valor.

---

## ⚠️ AVISOS IMPORTANTES

1. **Produção:** Este é um exemplo educacional. Para produção:
   - Adicione HTTPS/TLS
   - Use banco de dados persistente
   - Implemente rate limiting
   - Adicione logs de auditoria

2. **Dependências:** Requer OpenSSL 3.0+
   ```bash
   sudo apt-get install libssl-dev  # Ubuntu/Debian
   ```

3. **Firewall:** Certifique-se que a porta 8080 está aberta.

---

## 📚 PRÓXIMOS PASSOS

- **[FUNCOES.md](FUNCOES.md)** → Todas as 14 funções AGLE
- **[CHEAT_SHEET.md](CHEAT_SHEET.md)** → Exemplos rápidos
- **[exemplo_simples.c](exemplo_simples.c)** → Exemplo básico AGLE

---

## 🎉 RESULTADO

Você agora tem um **sistema completo de autenticação** com:

✅ Protocolo de comunicação HTTP/REST  
✅ Criptografia forte (SHAKE256 + KDF)  
✅ Interface web moderna  
✅ Tokens de sessão seguros  
✅ API REST documentada  
✅ Código limpo e comentado  

**Tudo pronto para usar! 🚀🔐**
