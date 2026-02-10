#!/bin/bash
# Script para criar usuário de teste automaticamente

echo "════════════════════════════════════════════════════════════"
echo "   🔧 CRIANDO USUÁRIO DE TESTE"
echo "════════════════════════════════════════════════════════════"
echo ""

API="http://localhost:8080"

# Verificar se servidor está ativo
echo "📡 Verificando servidor..."
if ! curl -s "$API/stats" > /dev/null 2>&1; then
    echo "❌ Servidor não está rodando!"
    echo ""
    echo "🚀 Inicie o servidor primeiro:"
    echo "   ./servidor_auth"
    echo ""
    exit 1
fi
echo "✅ Servidor ativo!"
echo ""

# Criar usuário de teste
echo "👤 Criando usuário de teste..."
echo "   Usuário: admin"
echo "   Senha: admin123"
echo ""

RESPONSE=$(curl -s -X POST "$API/register" \
  -H "Content-Type: application/json" \
  -d '{"username":"admin","password":"admin123"}')

echo "📨 Resposta do servidor:"
echo "$RESPONSE" | python3 -m json.tool 2>/dev/null || echo "$RESPONSE"
echo ""

# Verificar se foi criado com sucesso
if echo "$RESPONSE" | grep -q '"success":true'; then
    echo "════════════════════════════════════════════════════════════"
    echo "   ✅ USUÁRIO CRIADO COM SUCESSO!"
    echo "════════════════════════════════════════════════════════════"
    echo ""
    echo "🔑 CREDENCIAIS PARA LOGIN:"
    echo ""
    echo "   Usuário: admin"
    echo "   Senha:   admin123"
    echo ""
    echo "📝 Agora você pode:"
    echo "   1. Abrir cliente_auth.html no navegador"
    echo "   2. Ir na aba 'Login'"
    echo "   3. Digite: admin / admin123"
    echo "   4. Clique em 'Entrar'"
    echo ""
    echo "════════════════════════════════════════════════════════════"
elif echo "$RESPONSE" | grep -q "já existe"; then
    echo "════════════════════════════════════════════════════════════"
    echo "   ℹ️  USUÁRIO JÁ EXISTE!"
    echo "════════════════════════════════════════════════════════════"
    echo ""
    echo "🔑 Use estas credenciais para login:"
    echo ""
    echo "   Usuário: admin"
    echo "   Senha:   admin123"
    echo ""
    echo "════════════════════════════════════════════════════════════"
else
    echo "❌ Erro ao criar usuário!"
    echo ""
fi
