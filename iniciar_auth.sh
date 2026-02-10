#!/bin/bash

echo "════════════════════════════════════════════════════════════"
echo "   🚀 INICIANDO SISTEMA DE AUTENTICAÇÃO SEGURA"
echo "════════════════════════════════════════════════════════════"
echo ""

# Verificar se está compilado
if [ ! -f "./servidor_auth" ]; then
    echo "📦 Compilando servidor..."
    gcc -O2 -o servidor_auth servidor_auth.c agle.c -lssl -lcrypto -Wall
    
    if [ $? -ne 0 ]; then
        echo "❌ Erro na compilação!"
        exit 1
    fi
    echo "✅ Compilado!"
    echo ""
fi

# Iniciar servidor em background
echo "🔧 Iniciando servidor HTTP na porta 8080..."
./servidor_auth &
SERVER_PID=$!
echo "✅ Servidor iniciado! PID: $SERVER_PID"
echo ""

# Aguardar servidor inicializar
sleep 2

# Abrir navegador com o cliente
echo "🌐 Abrindo interface no navegador..."
if command -v firefox &> /dev/null; then
    firefox cliente_auth.html &
elif command -v google-chrome &> /dev/null; then
    google-chrome cliente_auth.html &
elif command -v xdg-open &> /dev/null; then
    xdg-open cliente_auth.html &
else
    echo "⚠️  Abra manualmente: cliente_auth.html"
fi

echo ""
echo "════════════════════════════════════════════════════════════"
echo "   ✅ SISTEMA ATIVO!"
echo "════════════════════════════════════════════════════════════"
echo ""
echo "📡 Servidor: http://localhost:8080"
echo "🌐 Interface: cliente_auth.html"
echo ""
echo "🔑 EXPERIMENTE:"
echo "   1. Crie uma conta na aba 'Registrar'"
echo "   2. Faça login na aba 'Login'"
echo "   3. Veja seu token seguro no 'Painel'"
echo ""
echo "⏹️  Para parar o servidor:"
echo "   kill $SERVER_PID"
echo "   ou pressione Ctrl+C no terminal do servidor"
echo ""
echo "════════════════════════════════════════════════════════════"
echo ""

# Aguardar
echo "⏳ Servidor rodando... (Ctrl+C para parar)"
wait $SERVER_PID
