#include "image_saver.h"
#include <SDL3_image/SDL_image.h>
#include <stdio.h>

#ifdef _WIN32
#include <io.h>
#define ARQUIVO_EXISTE(p) (_access(p, 0) == 0)
#else
#include <unistd.h>
#define ARQUIVO_EXISTE(p) (access(p, F_OK) == 0)
#endif

void salvar_imagem(SDL_Surface *imagem_atual) {
    const char *caminho_saida = "output_image.png";
    bool ja_existia = ARQUIVO_EXISTE(caminho_saida);

    if (IMG_SavePNG(imagem_atual, caminho_saida)) {
        printf("Arquivo '%s' %s.\n", caminho_saida, ja_existia ? "sobrescrito" : "criado");
    } else {
        fprintf(stderr, "Erro ao salvar imagem: %s\n", SDL_GetError());
    }
}
