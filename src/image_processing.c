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

    for (int i = 0; i < 256; i++) {
        histograma[i] = 0;
    }

    for (int y = 0; y < imagem->h; y++) {
        for (int x = 0; x < imagem->w; x++) {

            Uint8 r, g, b;

            if (!SDL_ReadSurfacePixel(imagem, x, y, &r, &g, &b, NULL)) {
                fprintf(stderr,
                        "Erro ao ler pixel (%d, %d): %s\n",
                        x, y, SDL_GetError());
                return;
            }

            histograma[r]++;
        }
    }
}

double calcular_media(
    const Uint32 histograma[256],
    Uint64 total_pixels
) {
    if (total_pixels == 0) {
        return 0.0;
    }

    double soma = 0.0;

    for (int i = 0; i < 256; i++) {
        soma += (double)i * histograma[i];
    }

    return soma / (double)total_pixels;
}

double calcular_desvio_padrao(
    const Uint32 histograma[256],
    Uint64 total_pixels,
    double media
) {
    if (total_pixels == 0) {
        return 0.0;
    }

    double soma = 0.0;

    for (int i = 0; i < 256; i++) {
        double diferenca = (double)i - media;

        soma +=
            diferenca *
            diferenca *
            histograma[i];
    }

    double variancia =
        soma / (double)total_pixels;

    return SDL_sqrt(variancia);
}

const char *classificar_intensidade(double media) {
    if (media < 85.0) {
        return "escura";
    }

    if (media < 170.0) {
        return "media";
    }

    return "clara";
}

const char *classificar_contraste(double desvio_padrao) {
    if (desvio_padrao < 42.5) {
        return "baixo";
    }

    if (desvio_padrao < 85.0) {
        return "medio";
    }

    return "alto";
}