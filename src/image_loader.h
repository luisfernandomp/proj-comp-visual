#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <SDL3/SDL.h>

/* Carrega uma imagem do disco. Retorna NULL em caso de erro (arquivo inexistente ou formato inválido) e imprime o motivo. */
SDL_Surface* carregar_imagem(const char *caminho);

#endif
