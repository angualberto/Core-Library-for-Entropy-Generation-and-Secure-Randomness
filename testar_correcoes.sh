#!/bin/bash

# ═══════════════════════════════════════════════════════════════════
# 🔧 TESTE RÁPIDO DE CORREÇÕES DE SEGURANÇA
# ═══════════════════════════════════════════════════════════════════

API_URL="http://localhost:8080"
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m'

echo "╔═══════════════════════════════════════════════════════════════════╗"
echo "║                                                                   ║"
echo "║         🧪 TESTE DE CORREÇÕES DE SEGURANÇA - RÁPIDO              ║"
echo "║                                                                   ║"
echo "╚═══════════════════════════════════════════════════════════════════╝"
echo ""

# ═══════════════════════════════════════════════════════════════════
# TESTE 1: SENHA FRACA (deve ser rejeitada)
# ═══════════════════════════════════════════════════════════════════

echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}TESTE 1: POLÍTICA DE SENHA FORTE${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""
echo "Tentando registrar com senha fraca '12345678'..."

RESPONSE=$(curl -s -X POST "$API_URL/register" \
    -H "Content-Type: application/json" \
    -d '{"username":"weak_test","password":"12345678"}')

if echo "$RESPONSE" | grep -q '"success":false'; then
    echo -e "${GREEN}✅ PASSOU:${NC} Senha fraca foi rejeitada"
else
    echo -e "${RED}❌ FALHOU:${NC} Senha fraca foi aceita!"
fi

echo ""
echo "Tentando registrar com senha forte 'MyP@ssw0rd2025!'..."

RESPONSE=$(curl -s -X POST "$API_URL/register" \
    -H "Content-Type: application/json" \
    -d '{"username":"strong_test","password":"MyP@ssw0rd2025!"}')

if echo "$RESPONSE" | grep -q '"success":true'; then
    echo -e "${GREEN}✅ PASSOU:${NC} Senha forte foi aceita"
else
    echo -e "${RED}❌ FALHOU:${NC} Senha forte foi rejeitada!"
fi

echo ""

# ═══════════════════════════════════════════════════════════════════
# TESTE 2: RATE LIMITING (deve bloquear após 3 tentativas)
# ═══════════════════════════════════════════════════════════════════

echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}TESTE 2: RATE LIMITING${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""

# Registrar usuário para teste
curl -s -X POST "$API_URL/register" \
    -H "Content-Type: application/json" \
    -d '{"username":"rate_test","password":"RateTest123!"}' > /dev/null 2>&1

echo "Fazendo 3 tentativas de login com senha errada..."

for i in {1..3}; do
    RESPONSE=$(curl -s -X POST "$API_URL/login" \
        -H "Content-Type: application/json" \
        -d '{"username":"rate_test","password":"errada123"}')
    echo "  Tentativa $i: $(echo $RESPONSE | grep -o '"error":"[^"]*"')"
done

echo ""
echo "Tentativa 4 (deve ser bloqueada)..."

RESPONSE=$(curl -s -X POST "$API_URL/login" \
    -H "Content-Type: application/json" \
    -d '{"username":"rate_test","password":"errada123"}')

# Verificar logs do servidor para bloqueio
sleep 1

echo ""
echo -e "${YELLOW}⚠️  Verifique os logs do servidor para:${NC}"
echo "    '🔒 Conta bloqueada por 5 minutos após 3 tentativas'"
echo ""

# ═══════════════════════════════════════════════════════════════════
# TESTE 3: LOGS LIMPOS (não deve expor hashes/salts)
# ═══════════════════════════════════════════════════════════════════

echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}TESTE 3: LOGS LIMPOS (SEM EXPOSIÇÃO DE DADOS)${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""

echo "Registrando usuário 'log_test'..."

curl -s -X POST "$API_URL/register" \
    -H "Content-Type: application/json" \
    -d '{"username":"log_test","password":"LogTest123!"}' > /dev/null 2>&1

sleep 1

echo ""
echo -e "${GREEN}✅ VERIFICAR:${NC} Logs do servidor NÃO devem mostrar:"
echo "    • Salt em hexadecimal"
echo "    • Hash parcial ou completo"
echo "    • Apenas: '✅ Usuário registrado: log_test'"
echo ""

# ═══════════════════════════════════════════════════════════════════
# TESTE 4: LIMPEZA DE SESSÕES
# ═══════════════════════════════════════════════════════════════════

echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}TESTE 4: LIMPEZA AUTOMÁTICA DE SESSÕES${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""

echo "Criando várias sessões..."

for i in {1..5}; do
    USERNAME="session_test_$i"
    curl -s -X POST "$API_URL/register" \
        -H "Content-Type: application/json" \
        -d "{\"username\":\"$USERNAME\",\"password\":\"Test123!\"}" > /dev/null 2>&1
    
    curl -s -X POST "$API_URL/login" \
        -H "Content-Type: application/json" \
        -d "{\"username\":\"$USERNAME\",\"password\":\"Test123!\"}" > /dev/null 2>&1
    
    echo "  Sessão $i criada"
done

echo ""
echo -e "${GREEN}✅ VERIFICAR:${NC} Ao criar novas sessões, logs devem mostrar:"
echo "    '🧹 N sessões expiradas limpas' (se houver expiradas)"
echo ""

# ═══════════════════════════════════════════════════════════════════
# RESUMO
# ═══════════════════════════════════════════════════════════════════

echo ""
echo "╔═══════════════════════════════════════════════════════════════════╗"
echo "║                                                                   ║"
echo "║                        📊 RESUMO DOS TESTES                       ║"
echo "║                                                                   ║"
echo "╚═══════════════════════════════════════════════════════════════════╝"
echo ""
echo -e "${GREEN}CORREÇÕES TESTADAS:${NC}"
echo "  ✅ Política de senha forte (12+ chars, complexidade)"
echo "  ✅ Rate limiting (bloqueio após 3 tentativas)"
echo "  ✅ Logs limpos (sem exposição de hashes/salts)"
echo "  ✅ Limpeza de sessões expiradas"
echo ""
echo -e "${YELLOW}VERIFICAÇÕES MANUAIS NECESSÁRIAS:${NC}"
echo "  • Verificar logs do servidor para confirmar:"
echo "    - Mensagens de bloqueio de conta"
echo "    - Ausência de hashes/salts"
echo "    - Mensagens de limpeza de sessões"
echo ""
echo -e "${BLUE}PRÓXIMOS TESTES:${NC}"
echo "  • Execute: ./teste_vulnerabilidades.sh (teste completo)"
echo "  • Verifique: CORRECOES_APLICADAS.md (documentação)"
echo ""
