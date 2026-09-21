#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdbool.h>
#include "image_loader.h"
#include "image_saver.h"
#include "gui.h"

/*
 * Ponto de entrada: carrega a imagem, cria a interface (janela principal +
 * janela secundária com histograma) e executa o loop de eventos.
 *
 * INTEGRAÇÃO PENDENTE: as funções marcadas com "TEMPORÁRIO" abaixo devem ser
 * substituídas pelos módulos de escala de cinza, análise e equalização do
 * histograma feitos pelos demais integrantes.
 */

/* Resoluções percorridas pelo botão "Alterar resolução" (a primeira é a inicial). */
static const struct { int w, h; } RESOLUCOES[] = {
    { GUI_PRINCIPAL_LARGURA, GUI_PRINCIPAL_ALTURA },
    { 800, 600 },
    { 1280, 960 },
};
#define NUM_RESOLUCOES (int)(sizeof(RESOLUCOES) / sizeof(RESOLUCOES[0]))

/* TEMPORÁRIO: calcula o histograma pela luminância de cada pixel, apenas para
 * a GUI ter dados reais até o módulo de histograma ser integrado. */
static bool calcular_histograma_temp(SDL_Surface *imagem, Uint32 histograma[GUI_NIVEIS]) {
    SDL_Surface *rgba = SDL_ConvertSurface(imagem, SDL_PIXELFORMAT_RGBA32);
    if (rgba == NULL) {
        fprintf(stderr, "Erro ao converter imagem: %s\n", SDL_GetError());
        return false;
    }

    for (int i = 0; i < GUI_NIVEIS; ++i) histograma[i] = 0;

    for (int y = 0; y < rgba->h; ++y) {
        const Uint8 *linha = (const Uint8 *)rgba->pixels + y * rgba->pitch;
        for (int x = 0; x < rgba->w; ++x) {
            const Uint8 *p = linha + x * 4;
            const Uint8 cinza = (Uint8)(0.299 * p[0] + 0.587 * p[1] + 0.114 * p[2]);
            histograma[cinza]++;
        }
    }
    SDL_DestroySurface(rgba);
    return true;
}

/* TEMPORÁRIO: monta as linhas de análise exibidas na janela secundária. */
static void montar_linhas_temp(const Uint32 histograma[GUI_NIVEIS], char linhas[][64], int *num_linhas) {
    double soma = 0.0, total = 0.0;
    for (int i = 0; i < GUI_NIVEIS; ++i) {
        soma  += (double)i * histograma[i];
        total += histograma[i];
    }
    const double media = total > 0 ? soma / total : 0.0;

    double variancia = 0.0;
    for (int i = 0; i < GUI_NIVEIS; ++i) {
        variancia += (i - media) * (i - media) * histograma[i];
    }
    const double desvio = total > 0 ? SDL_sqrt(variancia / total) : 0.0;

    snprintf(linhas[0], 64, "Intensidade média: %.2f", media);
    snprintf(linhas[1], 64, "Desvio padrão: %.2f", desvio);
    snprintf(linhas[2], 64, "Total de pixels: %.0f", total);
    *num_linhas = 3;
}

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

    Gui gui;
    if (!gui_iniciar(&gui, "Processamento de Imagens - Histograma")) {
        SDL_DestroySurface(imagem);
        SDL_Quit();
        return 1;
    }
    gui_definir_imagem(&gui, imagem);

    Uint32 histograma[GUI_NIVEIS];
    char   linhas[4][64];
    int    num_linhas = 0;
    calcular_histograma_temp(imagem, histograma);
    montar_linhas_temp(histograma, linhas, &num_linhas);

    const char *ponteiros_linhas[4] = { linhas[0], linhas[1], linhas[2], linhas[3] };
    GuiDadosSecundaria dados = {
        .histograma = histograma,
        .linhas     = ponteiros_linhas,
        .num_linhas = num_linhas,
        .equalizada = false,
    };

    int resolucao_atual = 0;
    bool executando = true;
    SDL_Event evento;

    while (executando) {
        while (SDL_PollEvent(&evento)) {
            switch (gui_processar_evento(&gui, &evento)) {
            case GUI_ACAO_SAIR:
                executando = false;
                break;

            case GUI_ACAO_EQUALIZAR:
                /* TEMPORÁRIO: aqui entra a equalização / retorno ao original.
                 * Após trocar a imagem: gui_definir_imagem(), recalcular o
                 * histograma e atualizar dados.equalizada. */
                dados.equalizada = !dados.equalizada;
                printf("Botão equalizar: %s\n", dados.equalizada ? "equalizar" : "voltar ao original");
                break;

            case GUI_ACAO_ALTERAR_RESOLUCAO:
                resolucao_atual = (resolucao_atual + 1) % NUM_RESOLUCOES;
                gui_definir_resolucao(&gui, RESOLUCOES[resolucao_atual].w, RESOLUCOES[resolucao_atual].h);
                break;

            case GUI_ACAO_SALVAR:
                salvar_imagem(imagem);
                break;

            case GUI_ACAO_NENHUMA:
                break;
            }
        }

        gui_desenhar_principal(&gui);
        gui_desenhar_secundaria(&gui, &dados);
        SDL_Delay(16);
    }

    gui_encerrar(&gui);
    SDL_DestroySurface(imagem);
    SDL_Quit();
    return 0;
}
