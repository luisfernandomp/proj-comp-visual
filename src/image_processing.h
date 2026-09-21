#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <SDL3/SDL.h>
#include <stdbool.h>

/*
 * Verifica se todos os pixels da imagem possuem
 * os canais R, G e B com o mesmo valor.
 *
 * Retorna true se a imagem estiver em escala de cinza
 * e false se possuir algum pixel colorido.
 */
bool imagem_eh_cinza(SDL_Surface *imagem);

#endif