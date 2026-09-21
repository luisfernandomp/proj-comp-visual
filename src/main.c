#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdbool.h>

#include "image_loader.h"
#include "image_saver.h"
#include "gui.h"
#include "image_processing.h"

/*
 * Ponto de entrada: carrega a imagem, converte para escala de cinza
 * quando necessário, calcula o histograma e as estatísticas da imagem,
 * cria a interface e executa o loop de eventos.
 *
 * INTEGRAÇÃO PENDENTE:
 * a equalização do histograma ainda será integrada posteriormente.
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
     * Isso garante um formato consistente para todas
     * as operações de processamento dos pixels.
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
     * Verifica se a imagem já está em escala de cinza.
     * Caso seja colorida, realiza a conversão usando
     * a fórmula definida no enunciado.
     */
    if (imagem_eh_cinza(imagem)) {
        printf("A imagem de entrada esta em escala de cinza.\n");
    } else {
        printf("A imagem de entrada e colorida.\n");

        converter_para_cinza(imagem);

        printf("Imagem convertida para escala de cinza.\n");
    }

    /*
     * Calcula o histograma da imagem já em escala de cinza.
     */
    Uint32 histograma[GUI_NIVEIS];

    calcular_histograma(
        imagem,
        histograma
    );

    /*
     * Quantidade total de pixels da imagem.
     */
    Uint64 total_pixels =
        (Uint64)imagem->w * (Uint64)imagem->h;

    /*
     * Calcula as estatísticas da imagem.
     */
    double media = calcular_media(
        histograma,
        total_pixels
    );

    double desvio_padrao = calcular_desvio_padrao(
        histograma,
        total_pixels,
        media
    );

    /*
     * Classifica a intensidade e o contraste.
     */
    const char *classificacao_intensidade =
        classificar_intensidade(media);

    const char *classificacao_contraste =
        classificar_contraste(desvio_padrao);

    /*
     * Monta as informações que serão exibidas
     * na janela secundária.
     */
    char linhas[4][64];

    snprintf(
        linhas[0],
        64,
        "Intensidade media: %.2f (%s)",
        media,
        classificacao_intensidade
    );

    snprintf(
        linhas[1],
        64,
        "Desvio padrao: %.2f (%s)",
        desvio_padrao,
        classificacao_contraste
    );

    snprintf(
        linhas[2],
        64,
        "Total de pixels: %llu",
        (unsigned long long)total_pixels
    );

    int num_linhas = 3;

    /*
     * Inicializa a interface gráfica.
     */
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
     * A imagem enviada para a GUI já está
     * em formato RGBA32 e escala de cinza.
     */
    gui_definir_imagem(&gui, imagem);

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
                 * aqui ainda entra a equalização / retorno ao original.
                 *
                 * Após integrar:
                 * - trocar a imagem;
                 * - atualizar a GUI;
                 * - recalcular o histograma;
                 * - recalcular média e desvio padrão;
                 * - atualizar as classificações.
                 */
                dados.equalizada =
                    !dados.equalizada;

                printf(
                    "Botao equalizar: %s\n",
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