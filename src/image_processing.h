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

/*
 * Converte uma imagem colorida para escala de cinza
 * usando a fórmula definida no enunciado:
 *
 * Y = 0.2125 * R + 0.7154 * G + 0.0721 * B
 */
void converter_para_cinza(SDL_Surface *imagem);

/*
 * Calcula o histograma de uma imagem em escala de cinza.
 *
 * O vetor possui 256 posições:
 * histograma[0] representa a quantidade de pixels pretos,
 * histograma[255] representa a quantidade de pixels brancos,
 * e as demais posições representam os níveis intermediários.
 */
void calcular_histograma(
    SDL_Surface *imagem,
    Uint32 histograma[256]
);

#endif