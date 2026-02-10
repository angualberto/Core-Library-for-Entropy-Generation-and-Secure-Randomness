# AGLE - Alpha-Gauss-Logistic Entropy Generator

AGLE (Alpha-Gauss-Logistic Entropy Generator) e uma biblioteca C de entropia
criptografica para RNG, senhas, hashing e derivacao de chaves, com foco em
orquestracao de entropia e reproducao experimental de dinamica caotica.

## Descricao

AGLE combina fontes de entropia do sistema com processamento criptografico
(SHAKE256 via OpenSSL) para gerar bytes aleatorios, tokens e hashes. O projeto
representa pesquisa em CSPRNGs com enfase em irreversibilidade induzida por
hardware em sistemas caoticos.

## Motivacao e contexto

- Estudar o efeito de realimentacao caotica em pipelines de entropia.
- Documentar um artefato experimental reprodutivel para pesquisa.
- Disponibilizar uma API C pequena e clara para uso academico.

## Arquitetura geral

- Fontes de entropia: /dev/urandom (Linux) e, quando integrado, feedback
  de mapas caoticos externos.
- Mistura e expansao: SHAKE256 (OpenSSL) para difusao e expansao.
- API publica: RNG, senhas, hashing, KDF, tokens e utilitarios.

AGLE nao substitui o OpenSSL. O OpenSSL fornece primitivas criptograficas, e o
AGLE coordena fontes de entropia e o pipeline de mistura.

## Estrutura do projeto

```
include/         # headers publicos
src/             # implementacao da biblioteca
examples/        # exemplos de uso
tests/           # testes e utilitarios de validacao
build/           # artefatos de compilacao
```

## Exemplo de uso

```c
#include "agle.h"
#include <stdio.h>

int main(void) {
    AGLE_CTX ctx;
    if (!AGLE_Init(&ctx)) {
        return 1;
    }

    char password[33];
    AGLE_GeneratePassword(&ctx, AGLE_CHARSET_ALL, 32, password);
    printf("Password: %s\n", password);

    AGLE_Cleanup(&ctx);
    return 0;
}
```

## Instalacao

### CMake (recomendado)

```bash
cmake -S . -B build
cmake --build build
sudo cmake --install build
```

### Makefile (alternativo)

```bash
make all
sudo make install
```

### pkg-config

```bash
pkg-config --cflags --libs agle
```

## Requisitos

- Compilador C99 (GCC ou Clang)
- OpenSSL 3.0+
- Linux (principal); macOS pode exigir ajustes de entropia

## Licenca

Este projeto e licenciado sob Apache-2.0. Consulte [LICENSE](LICENSE) para
detalhes.

## Citacao

```bibtex
@misc{gualberto2025agle,
  author = {Gualberto, Andre},
  title = {AGLE: Cryptographic Entropy Generator Based on Chaotic Dynamics with Floating-Point Error Feedback},
  year = {2025},
  publisher = {GitHub},
  url = {https://github.com/angualberto/Hardware-Induced-Irreversibility-in-Chaotic-Maps}
}
```

## Aviso legal

Este software e fornecido "como esta", sem garantias. O uso em producao exige
avaliacao independente de seguranca.
