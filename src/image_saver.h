#ifndef IMAGE_SAVER_H
#define IMAGE_SAVER_H

#include <SDL3/SDL.h>

/* Salva a imagem atualmente exibida em output_image.png, sobrescrevendo se já existir. */
void salvar_imagem(SDL_Surface *imagem_atual);

#endif
