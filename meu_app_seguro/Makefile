CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O3 -march=native
LIBS = -lssl -lcrypto

# Detectar se já tem a library instalada
AGLE_LIB = $(shell pkg-config --cflags --libs libssl libcrypto 2>/dev/null || echo "-lssl -lcrypto")

TARGET = meu_app
SOURCES = main.c agle.c
OBJECTS = $(SOURCES:.c=.o)
HEADER = agle.h

# Compile options
ifdef DEBUG
    CFLAGS = -Wall -Wextra -std=c99 -g -O0 -DDEBUG
endif

ifdef RELEASE
    CFLAGS = -Wall -Wextra -std=c99 -O3 -march=native -DNDEBUG
endif

.PHONY: all clean run help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "🔗 Ligando: $@"
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "✅ Compilado: ./$@"

%.o: %.c $(HEADER)
	@echo "📦 Compilando: $<"
	$(CC) $(CFLAGS) -c $<

run: $(TARGET)
	@echo "🚀 Executando .../$(TARGET)..."
	./$(TARGET)

debug:
	@echo "🐛 Compilando com debug..."
	$(MAKE) DEBUG=1 clean all

release:
	@echo "⚡ Compilando versão release..."
	$(MAKE) RELEASE=1 clean all
	strip $(TARGET)

clean:
	@echo "🗑️  Limpando..."
	rm -f *.o $(TARGET)
	@echo "✅ Limpo"

help:
	@echo "Comandos disponíveis:"
	@echo "  make           - Compilar (padrão)"
	@echo "  make run       - Compilar e executar"
	@echo "  make debug     - Compilar com debug (-g)"
	@echo "  make release   - Versão otimizada"
	@echo "  make clean     - Remover binários"
	@echo "  make help      - Este menu"
