#include "image_processing.h"
#include <stdio.h>

bool imagem_eh_cinza(SDL_Surface *imagem) {
    if (imagem == NULL) {
        fprintf(stderr, "Erro: imagem nula ao verificar escala de cinza.\n");
        return false;
    }

    for (int y = 0; y < imagem->h; y++) {
        for (int x = 0; x < imagem->w; x++) {

            Uint8 r, g, b;

            if (!SDL_ReadSurfacePixel(imagem, x, y, &r, &g, &b, NULL)) {
                fprintf(stderr,
                        "Erro ao ler pixel (%d, %d): %s\n",
                        x, y, SDL_GetError());
                return false;
            }

            if (r != g || g != b) {
                return false;
            }
        }
    }

    return true;
}

void converter_para_cinza(SDL_Surface *imagem) {
    if (imagem == NULL) {
        fprintf(stderr, "Erro: imagem nula ao converter para escala de cinza.\n");
        return;
    }

    for (int y = 0; y < imagem->h; y++) {
        for (int x = 0; x < imagem->w; x++) {

            Uint8 r, g, b, a;

            if (!SDL_ReadSurfacePixel(imagem, x, y, &r, &g, &b, &a)) {
                fprintf(stderr,
                        "Erro ao ler pixel (%d, %d): %s\n",
                        x, y, SDL_GetError());
                return;
            }

            Uint8 cinza = (Uint8)(
                0.2125 * r +
                0.7154 * g +
                0.0721 * b
            );

            if (!SDL_WriteSurfacePixel(imagem, x, y, cinza, cinza, cinza, a)) {
                fprintf(stderr,
                        "Erro ao escrever pixel (%d, %d): %s\n",
                        x, y, SDL_GetError());
                return;
            }
        }
    }
}

void calcular_histograma(
    SDL_Surface *imagem,
    Uint32 histograma[256]
) {
    if (imagem == NULL) {
        fprintf(stderr, "Erro: imagem nula ao calcular histograma.\n");
        return;
    }

    /*
     * Inicializa todas as posições do histograma com zero.
     */
    for (int i = 0; i < 256; i++) {
        histograma[i] = 0;
    }

    /*
     * Percorre todos os pixels da imagem.
     */
    for (int y = 0; y < imagem->h; y++) {
        for (int x = 0; x < imagem->w; x++) {

            Uint8 r, g, b;

            if (!SDL_ReadSurfacePixel(imagem, x, y, &r, &g, &b, NULL)) {
                fprintf(stderr,
                        "Erro ao ler pixel (%d, %d): %s\n",
                        x, y, SDL_GetError());
                return;
            }

            /*
             * A imagem já está em escala de cinza neste ponto,
             * portanto R, G e B possuem o mesmo valor.
             *
             * O canal R é usado como intensidade do pixel.
             */
            histograma[r]++;
        }
    }
}