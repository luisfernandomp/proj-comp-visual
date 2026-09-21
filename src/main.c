#include <SDL3/SDL.h>
#include "image_loader.h"
#include "image_saver.h"
#include <stdio.h>

/* main.c de teste isolado para minha parte. Será mesclado com a GUI (janelas, loop de eventos, botões) que o restante do grupo fez. */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <caminho_da_imagem>\n", argv[0]);
        return 1;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *imagem = carregar_imagem(argv[1]);
    if (imagem == NULL) {
        SDL_Quit();
        return 1;
    }

    salvar_imagem(imagem);

    SDL_DestroySurface(imagem);
    SDL_Quit();
    return 0;
}
