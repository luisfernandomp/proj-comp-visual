#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdbool.h>

#include "image_loader.h"
#include "image_saver.h"
#include "gui.h"
#include "image_processing.h"

/*
 * Ponto de entrada: carrega a imagem, cria a interface (janela principal +
 * janela secundária com histograma) e executa o loop de eventos.
 *
 * INTEGRAÇÃO PENDENTE: as funções marcadas com "TEMPORÁRIO" abaixo devem ser
 * substituídas pelos módulos de análise e equalização do histograma.
 */

/* Resoluções percorridas pelo botão "Alterar resolução" (a primeira é a inicial). */
static const struct {
    int w, h;
} RESOLUCOES[] = {
    { GUI_PRINCIPAL_LARGURA, GUI_PRINCIPAL_ALTURA },
    { 800, 600 },
    { 1280, 960 },
};

#define NUM_RESOLUCOES (int)(sizeof(RESOLUCOES) / sizeof(RESOLUCOES[0]))

/*
 * TEMPORÁRIO:
 * monta as linhas de análise exibidas na janela secundária.
 *
 * Esta função ainda será substituída pelas funções definitivas
 * de média, desvio padrão e classificação da imagem.
 */
static void montar_linhas_temp(
    const Uint32 histograma[GUI_NIVEIS],
    char linhas[][64],
    int *num_linhas
) {
    double soma = 0.0;
    double total = 0.0;

    for (int i = 0; i < GUI_NIVEIS; ++i) {
        soma += (double)i * histograma[i];
        total += histograma[i];
    }

    const double media =
        total > 0 ? soma / total : 0.0;

    double variancia = 0.0;

    for (int i = 0; i < GUI_NIVEIS; ++i) {
        variancia +=
            (i - media) *
            (i - media) *
            histograma[i];
    }

    const double desvio =
        total > 0 ? SDL_sqrt(variancia / total) : 0.0;

    snprintf(
        linhas[0],
        64,
        "Intensidade média: %.2f",
        media
    );

    snprintf(
        linhas[1],
        64,
        "Desvio padrão: %.2f",
        desvio
    );

    snprintf(
        linhas[2],
        64,
        "Total de pixels: %.0f",
        total
    );

    *num_linhas = 3;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(
            stderr,
            "Uso: %s <caminho_da_imagem>\n",
            argv[0]
        );

        return 1;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(
            stderr,
            "Erro ao inicializar SDL: %s\n",
            SDL_GetError()
        );

        return 1;
    }

    /*
     * Carrega a imagem no formato original.
     */
    SDL_Surface *imagem_carregada = carregar_imagem(argv[1]);

    if (imagem_carregada == NULL) {
        SDL_Quit();
        return 1;
    }

    /*
     * Converte a superfície para RGBA32.
     *
     * Isso garante que o processamento dos pixels seja feito
     * sobre um formato consistente, evitando problemas com
     * imagens indexadas/paletas, como imagens de 8 bpp.
     */
    SDL_Surface *imagem = SDL_ConvertSurface(
        imagem_carregada,
        SDL_PIXELFORMAT_RGBA32
    );

    /*
     * A superfície original não será mais utilizada.
     */
    SDL_DestroySurface(imagem_carregada);

    if (imagem == NULL) {
        fprintf(
            stderr,
            "Erro ao converter formato da imagem: %s\n",
            SDL_GetError()
        );

        SDL_Quit();
        return 1;
    }

    /*
     * Verifica se a imagem de entrada já está em escala de cinza.
     * Caso seja colorida, realiza a conversão usando a fórmula
     * definida no enunciado.
     */
    if (imagem_eh_cinza(imagem)) {
        printf("A imagem de entrada esta em escala de cinza.\n");
    } else {
        printf("A imagem de entrada e colorida.\n");

        converter_para_cinza(imagem);

        printf("Imagem convertida para escala de cinza.\n");
    }

    Gui gui;

    if (!gui_iniciar(
            &gui,
            "Processamento de Imagens - Histograma"
        )) {

        SDL_DestroySurface(imagem);
        SDL_Quit();
        return 1;
    }

    /*
     * A partir daqui, a imagem utilizada pela interface
     * está em formato RGBA32 e em escala de cinza.
     */
    gui_definir_imagem(&gui, imagem);

    Uint32 histograma[GUI_NIVEIS];
    char linhas[4][64];
    int num_linhas = 0;

    /*
     * Calcula o histograma usando o módulo definitivo
     * de processamento de imagem.
     */
    calcular_histograma(
        imagem,
        histograma
    );

    montar_linhas_temp(
        histograma,
        linhas,
        &num_linhas
    );

    const char *ponteiros_linhas[4] = {
        linhas[0],
        linhas[1],
        linhas[2],
        linhas[3]
    };

    GuiDadosSecundaria dados = {
        .histograma = histograma,
        .linhas = ponteiros_linhas,
        .num_linhas = num_linhas,
        .equalizada = false,
    };

    int resolucao_atual = 0;
    bool executando = true;
    SDL_Event evento;

    while (executando) {
        while (SDL_PollEvent(&evento)) {

            switch (
                gui_processar_evento(
                    &gui,
                    &evento
                )
            ) {

            case GUI_ACAO_SAIR:
                executando = false;
                break;

            case GUI_ACAO_EQUALIZAR:

                /*
                 * TEMPORÁRIO:
                 * aqui entra a equalização / retorno ao original.
                 *
                 * Depois:
                 * - trocar a imagem
                 * - gui_definir_imagem()
                 * - recalcular o histograma
                 * - atualizar dados.equalizada
                 */
                dados.equalizada =
                    !dados.equalizada;

                printf(
                    "Botão equalizar: %s\n",
                    dados.equalizada
                        ? "equalizar"
                        : "voltar ao original"
                );

                break;

            case GUI_ACAO_ALTERAR_RESOLUCAO:

                resolucao_atual =
                    (resolucao_atual + 1)
                    % NUM_RESOLUCOES;

                gui_definir_resolucao(
                    &gui,
                    RESOLUCOES[resolucao_atual].w,
                    RESOLUCOES[resolucao_atual].h
                );

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