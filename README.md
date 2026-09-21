# Proj1 — Processamento de Imagens (Computação Visual)

## Integrantes
- Gabriel de Santana (RA: 10420595) — carregamento de imagem, sistema de build, salvamento de imagem
- André Cizotti (RA: 10409439) - processamento de imagem: detecção e conversão para escala de cinza, histograma e estatísticas


## Descrição
Programa em C com SDL3/SDL_image para carregar, analisar, equalizar e exibir imagens via linha de comando.

## Como compilar e executar

Requisitos: GCC (C99+), SDL3, SDL3_image e SDL3_ttf instaladas e visíveis pelo `pkg-config`.

```bash
make
./proj1 caminho_da_imagem.png
```

No Windows/MSYS2, o executável gerado é `proj1.exe`.
No ambiente Windows/MSYS2 UCRT64 utilizado por André, o comando de compilação é `mingw32-make` e execução pode ser feita com: `./proj1.exe caminho_da_imagem.png`

### Configuração de cada integrante

**Gabriel — Linux Mint 22.2**
- GCC 14.3.0 (via PPA ubuntu-toolchain-r/test)
- SDL3 e SDL3_image compiladas manualmente da fonte

**André - Windows 11 / MSYS2 UCRT64**
- GCC 16.2.0
- GNU Make 4.4.1
- pkg-config 3.0.7
- SDL3_image 3.4.6
- SDL3_ttf 3.2.2

**completar SO, compilador e bibliotecas de cada um aqui**


## Estrutura do projeto
```
proj1/
├── src/
│   ├── gui.h                  # interface gráfica
│   ├── gui.c
│   ├── image_loader.h         # carregamento de imagem
│   ├── image_loader.c
│   ├── image_processing.h     # processamento e análise da imagem
│   ├── image_processing.c
│   ├── image_saver.h          # salvamento de imagem
│   ├── image_saver.c
│   └── main.c                 # ponto de entrada e integração dos módulos
├── assets/
│   └── fonts/
├── Makefile                   # compila os arquivos em src/*.c
└── README.md
```

## Critérios de classificação

Como o enunciado não define valores de corte, foram adotados limiares próprios de implementação.

Para a intensidade média:
- `< 85`: imagem escura
- `85 a < 170`: imagem média
- `>= 170`: imagem clara

Para o contraste, com base no desvio padrão:
- `< 42,5`: baixo
- `42,5 a < 85`: médio
- `>= 85`: alto

## Contribuições
| Integrante | Parte |
|---|---|
| Gabriel de Santana (RA: 10420595) | Carregamento de imagem (leitura via `IMG_Load` com tratamento de erro para arquivo inexistente ou formato inválido), sistema de build multiplataforma (Makefile compatível com Linux e Windows/MSYS2) e salvamento de imagem (gravação em `output_image.png` via `IMG_SavePNG`, com verificação se o arquivo já existia para diferenciar criação de sobrescrita) |
| André Cizotti (RA: 10409439) | Processamento de imagem (verificação dos canais RGB para identificar se a imagem já está em escala de cinza, conversão de imagens coloridas utilizando a fórmula `Y = 0.2125 * R + 0.7154 * G + 0.0721 * B`, conversão da superfície para `RGBA32` para garantir consistência no acesso aos pixels, cálculo do histograma de 256 níveis de intensidade, cálculo da média e do desvio padrão das intensidades e classificação da imagem como clara, média ou escura e do contraste como alto, médio ou baixo) |
