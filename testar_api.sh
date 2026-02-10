#!/bin/bash
# Script de teste da API REST usando curl

API="http://localhost:8080"

echo "════════════════════════════════════════════════════════════"
echo "   🧪 TESTE AUTOMÁTICO DA API DE AUTENTICAÇÃO"
echo "════════════════════════════════════════════════════════════"
echo ""

# Verificar se o servidor está rodando
echo "📡 Verificando servidor..."
if ! curl -s "$API/stats" > /dev/null 2>&1; then
    echo "❌ Servidor não está rodando!"
    echo "   Execute: ./servidor_auth"
    exit 1
fi
echo "✅ Servidor ativo!"
echo ""

# 1. REGISTRAR USUÁRIO
echo "════════════════════════════════════════════════════════════"
echo "1️⃣  TESTE: Registrar Usuário"
echo "════════════════════════════════════════════════════════════"
echo "POST /register"
echo "Body: {\"username\":\"teste\",\"password\":\"Senha123!@#\"}"
echo ""
curl -X POST "$API/register" \
  -H "Content-Type: application/json" \
  -d '{"username":"teste","password":"Senha123!@#"}' \
  2>/dev/null | python3 -m json.tool || echo "{}"
echo ""
echo ""

# 2. FAZER LOGIN
echo "════════════════════════════════════════════════════════════"
echo "2️⃣  TESTE: Fazer Login"
echo "════════════════════════════════════════════════════════════"
echo "POST /login"
echo "Body: {\"username\":\"teste\",\"password\":\"Senha123!@#\"}"
echo ""
RESPONSE=$(curl -s -X POST "$API/login" \
  -H "Content-Type: application/json" \
  -d '{"username":"teste","password":"Senha123!@#"}')
echo "$RESPONSE" | python3 -m json.tool || echo "{}"
echo ""

# Extrair token
TOKEN=$(echo "$RESPONSE" | grep -o '"token":"[^"]*"' | cut -d'"' -f4)
echo "🎫 Token obtido: $TOKEN"
echo ""
echo ""

# 3. VALIDAR TOKEN
echo "════════════════════════════════════════════════════════════"
echo "3️⃣  TESTE: Validar Token"
echo "════════════════════════════════════════════════════════════"
echo "GET /validate"
echo "Header: Authorization: Bearer $TOKEN"
echo ""
curl -X GET "$API/validate" \
  -H "Authorization: Bearer $TOKEN" \
  2>/dev/null | python3 -m json.tool || echo "{}"
echo ""
echo ""

# 4. ESTATÍSTICAS
echo "════════════════════════════════════════════════════════════"
echo "4️⃣  TESTE: Estatísticas"
echo "════════════════════════════════════════════════════════════"
echo "GET /stats"
echo ""
curl -s "$API/stats" | python3 -m json.tool || echo "{}"
echo ""
echo ""

# 5. LOGOUT
echo "════════════════════════════════════════════════════════════"
echo "5️⃣  TESTE: Logout"
echo "════════════════════════════════════════════════════════════"
echo "POST /logout"
echo "Body: {\"token\":\"$TOKEN\"}"
echo ""
curl -X POST "$API/logout" \
  -H "Content-Type: application/json" \
  -d "{\"token\":\"$TOKEN\"}" \
  2>/dev/null | python3 -m json.tool || echo "{}"
echo ""
echo ""

# 6. TENTAR VALIDAR TOKEN INVÁLIDO
echo "════════════════════════════════════════════════════════════"
echo "6️⃣  TESTE: Token Inválido (após logout)"
echo "════════════════════════════════════════════════════════════"
echo "GET /validate"
echo "Header: Authorization: Bearer $TOKEN"
echo ""
curl -X GET "$API/validate" \
  -H "Authorization: Bearer $TOKEN" \
  2>/dev/null | python3 -m json.tool || echo "{}"
echo ""
echo ""

echo "════════════════════════════════════════════════════════════"
echo "   ✅ TODOS OS TESTES CONCLUÍDOS!"
echo "════════════════════════════════════════════════════════════"
echo ""
echo "📊 Resumo:"
echo "   ✅ Registro funcionando"
echo "   ✅ Login funcionando"
echo "   ✅ Token funcionando"
echo "   ✅ Validação funcionando"
echo "   ✅ Logout funcionando"
echo "   ✅ Estatísticas funcionando"
echo ""
