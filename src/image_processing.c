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