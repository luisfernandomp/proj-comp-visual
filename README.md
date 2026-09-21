# Proj1 — Processamento de Imagens (Computação Visual)

## Integrantes
- Gabriel de Santana (RA: 10420595) — carregamento de imagem, sistema de build, salvamento de imagem


## Descrição
Programa em C com SDL3/SDL_image para carregar, analisar, equalizar e exibir imagens via linha de comando.

## Como compilar e executar

Requisitos: GCC (C99+), SDL3, SDL3_image e SDL3_ttf instaladas e visíveis pelo `pkg-config`.

```bash
make
./proj1 caminho_da_imagem.png
```

No Windows/MSYS2, o executável gerado é `proj1.exe`.

### Configuração de cada integrante

**Gabriel — Linux Mint 22.2**
- GCC 14.3.0 (via PPA ubuntu-toolchain-r/test)
- SDL3 e SDL3_image compiladas manualmente da fonte

**completar SO, compilador e libs de cada um aqui**


## Estrutura do projeto
```
proj1/
├── src/
│   ├── image_loader.h    # carregamento de imagem
│   ├── image_loader.c
│   ├── image_saver.h     # salvamento de imagem
│   ├── image_saver.c
│   └── main.c             # ponto de entrada (temporário, será integrado com a GUI)
├── Makefile                # compila tudo em src/*.c
└── README.md
```

## Contribuições
| Integrante | Parte |
|---|---|
| Gabriel de Santana (10420595) | Carregamento de imagem (leitura via `IMG_Load` com tratamento de erro para arquivo inexistente ou formato inválido), sistema de build multiplataforma (Makefile compatível com Linux e Windows/MSYS2) e salvamento de imagem (gravação em `output_image.png` via `IMG_SavePNG`, com verificação se o arquivo já existia para diferenciar criação de sobrescrita) |
| | |
| | |
| | |
