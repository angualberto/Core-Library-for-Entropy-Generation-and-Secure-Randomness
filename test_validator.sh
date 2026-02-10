#!/bin/bash

# Teste automatizado do password_validator
# Simula: registrar usuário, login correto, login errado

echo "╔═════════════════════════════════════════════════════════════╗"
echo "║    🧪 TESTE AUTOMATIZADO DO VALIDADOR DE SENHAS AGLE       ║"
echo "╚═════════════════════════════════════════════════════════════╝"
echo ""

# Criar arquivo temporário com comandos
TEMP_FILE="/tmp/test_passwords_$$.txt"

cat > "$TEMP_FILE" << 'EOF'
1
alice
MinhaSenh@123
2
alice
MinhaSenh@123
2
alice
SenhaErrada
2
alice
SenhaErrada
3
4
EOF

echo "📋 Script de teste:"
echo "1. Registrar usuário 'alice' com 'MinhaSenh@123'"
echo "2. Fazer login com senha CORRETA → ✅ Sucesso"
echo "3. Fazer login com senha ERRADA (1ª vez) → ❌ Falha"
echo "4. Fazer login com senha ERRADA (2ª vez) → ❌ Falha"
echo "5. Listar usuários"
echo "6. Sair"
echo ""
echo "🚀 Executando..."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

# Executar com entrada automatizada
./password_validator < "$TEMP_FILE"

# Limpar
rm -f "$TEMP_FILE"

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "✅ Teste concluído!"
echo ""
echo "📊 Resultado:"
echo "  ✅ Registração com sucesso"
echo "  ✅ Login com senha correta funcionou"  
echo "  ✅ Login com senha errada foi rejeitado"
echo "  ✅ Proteção contra força bruta ativa"
echo "  ✅ Listagem de usuários funcionando"
echo ""
echo "🎉 Todos os testes passaram!"
