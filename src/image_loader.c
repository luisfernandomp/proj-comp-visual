#include "image_loader.h"
#include <SDL3_image/SDL_image.h>
#include <stdio.h>

SDL_Surface* carregar_imagem(const char *caminho) {
    if (caminho == NULL) {
        fprintf(stderr, "Erro: nenhum caminho de imagem foi informado.\n");
        return NULL;
    }

    SDL_Surface *imagem = IMG_Load(caminho);
    if (imagem == NULL) {
        fprintf(stderr, "Erro ao carregar '%s': %s\n", caminho, SDL_GetError());
        return NULL;
    }

    printf("Imagem '%s' carregada (%dx%d, %d bpp).\n",
           caminho, imagem->w, imagem->h, SDL_BITSPERPIXEL(imagem->format));

    return imagem;
}
