#include "gui.h"
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------------- */
/* Constantes de layout e cores                                              */
/* ------------------------------------------------------------------------- */

#define FONTE_ARQUIVO        "assets/fonts/DejaVuSans.ttf"
#define FONTE_TAMANHO        16.0f
#define FONTE_TAMANHO_PEQ    12.0f

/* Área do gráfico do histograma dentro da janela secundária.
 * Largura 512 = 2 px por nível de cinza (256 níveis). */
enum {
    HIST_X        = 24,
    HIST_Y        = 48,
    HIST_LARGURA  = 512,
    HIST_ALTURA   = 200,
    HIST_RAMPA_H  = 8,     /* faixa de gradiente 0..255 abaixo do eixo x */
    INFO_Y        = HIST_Y + HIST_ALTURA + HIST_RAMPA_H + 34,
    INFO_ESPACO   = 22,    /* distância vertical entre linhas de análise */
    BOTAO_LARGURA = 220,
    BOTAO_ALTURA  = 40,
    BOTAO_Y       = GUI_SECUNDARIA_ALTURA - BOTAO_ALTURA - 20,
    MARGEM        = 24,
};

static const SDL_Color COR_FUNDO_PRINCIPAL  = {  30,  30,  30, 255 };
static const SDL_Color COR_FUNDO_SECUNDARIA = {  24,  26,  32, 255 };
static const SDL_Color COR_PAINEL           = {  38,  41,  50, 255 };
static const SDL_Color COR_GRADE            = {  60,  64,  76, 255 };
static const SDL_Color COR_BARRA            = { 100, 170, 255, 255 };
static const SDL_Color COR_TEXTO            = { 235, 235, 235, 255 };
static const SDL_Color COR_TEXTO_SUAVE      = { 160, 165, 180, 255 };
static const SDL_Color COR_BOTAO            = {  58,  63,  78, 255 };
static const SDL_Color COR_BOTAO_HOVER      = {  84,  92, 116, 255 };
static const SDL_Color COR_BOTAO_PRESSIONADO = { 42, 46, 58, 255 };
static const SDL_Color COR_BOTAO_BORDA      = { 120, 130, 160, 255 };

/* ------------------------------------------------------------------------- */
/* Utilitários internos                                                      */
/* ------------------------------------------------------------------------- */

static void definir_cor(SDL_Renderer *render, SDL_Color cor) {
    SDL_SetRenderDrawColor(render, cor.r, cor.g, cor.b, cor.a);
}

/* Tenta abrir a fonte a partir do diretório do executável e, como
 * alternativa, a partir do diretório de trabalho atual. */
static TTF_Font *abrir_fonte(float tamanho) {
    const char *base = SDL_GetBasePath();
    TTF_Font *fonte = NULL;

    if (base != NULL) {
        char caminho[1024];
        SDL_snprintf(caminho, sizeof(caminho), "%s%s", base, FONTE_ARQUIVO);
        fonte = TTF_OpenFont(caminho, tamanho);
    }
    if (fonte == NULL) {
        fonte = TTF_OpenFont(FONTE_ARQUIVO, tamanho);
    }
    if (fonte == NULL) {
        fprintf(stderr, "Erro ao carregar a fonte '%s': %s\n", FONTE_ARQUIVO, SDL_GetError());
    }
    return fonte;
}

static bool criar_janela_principal(Gui *gui, const char *titulo) {
    SDL_DisplayID monitor = SDL_GetPrimaryDisplay();
    SDL_PropertiesID props = SDL_CreateProperties();

    SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, titulo);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER,  GUI_PRINCIPAL_LARGURA);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, GUI_PRINCIPAL_ALTURA);
    /* centralizada no monitor principal */
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED_DISPLAY(monitor));
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED_DISPLAY(monitor));

    gui->janela_principal = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);

    if (gui->janela_principal == NULL) {
        fprintf(stderr, "Erro ao criar a janela principal: %s\n", SDL_GetError());
        return false;
    }

    gui->render_principal = SDL_CreateRenderer(gui->janela_principal, NULL);
    if (gui->render_principal == NULL) {
        fprintf(stderr, "Erro ao criar o renderizador principal: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

static bool criar_janela_secundaria(Gui *gui) {
    SDL_PropertiesID props = SDL_CreateProperties();

    SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Histograma");
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER,  GUI_SECUNDARIA_LARGURA);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, GUI_SECUNDARIA_ALTURA);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, 0);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, 0);
    /* filha da janela principal: fica sempre acima dela e é fechada/minimizada junto */
    SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_PARENT_POINTER, gui->janela_principal);

    gui->janela_secundaria = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);

    if (gui->janela_secundaria == NULL) {
        fprintf(stderr, "Erro ao criar a janela secundária: %s\n", SDL_GetError());
        return false;
    }

    /* A posição da SDL refere-se à área cliente; em (0,0) a barra de título
     * ficaria fora da tela e a janela não poderia ser movida. Desloca-se pela
     * espessura da borda para que o quadro inteiro encoste no canto (0,0). */
    int topo = 0, esquerda = 0, base = 0, direita = 0;
    if (SDL_GetWindowBordersSize(gui->janela_secundaria, &topo, &esquerda, &base, &direita)) {
        SDL_SetWindowPosition(gui->janela_secundaria, esquerda, topo);
        SDL_SyncWindow(gui->janela_secundaria);
    }

    gui->render_secundario = SDL_CreateRenderer(gui->janela_secundaria, NULL);
    if (gui->render_secundario == NULL) {
        fprintf(stderr, "Erro ao criar o renderizador secundário: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

static bool botao_contem(const GuiBotao *botao, float x, float y) {
    const SDL_FPoint p = { x, y };
    return SDL_PointInRectFloat(&p, &botao->rect);
}

static void botao_desenhar(Gui *gui, const GuiBotao *botao) {
    SDL_Renderer *render = gui->render_secundario;

    if (botao->pressionado) {
        definir_cor(render, COR_BOTAO_PRESSIONADO);
    } else if (botao->hover) {
        definir_cor(render, COR_BOTAO_HOVER);
    } else {
        definir_cor(render, COR_BOTAO);
    }
    SDL_RenderFillRect(render, &botao->rect);
    definir_cor(render, COR_BOTAO_BORDA);
    SDL_RenderRect(render, &botao->rect);

    /* rótulo centralizado no botão */
    int tw = 0, th = 0;
    TTF_GetStringSize(gui->fonte, botao->rotulo, 0, &tw, &th);
    gui_desenhar_texto(render, gui->fonte, botao->rotulo,
                       botao->rect.x + (botao->rect.w - tw) / 2.0f,
                       botao->rect.y + (botao->rect.h - th) / 2.0f,
                       COR_TEXTO);
}

/* ------------------------------------------------------------------------- */
/* API pública                                                               */
/* ------------------------------------------------------------------------- */

bool gui_iniciar(Gui *gui, const char *titulo) {
    memset(gui, 0, sizeof(*gui));

    if (!TTF_Init()) {
        fprintf(stderr, "Erro ao iniciar SDL_ttf: %s\n", SDL_GetError());
        return false;
    }

    gui->fonte = abrir_fonte(FONTE_TAMANHO);
    if (gui->fonte != NULL) {
        gui->fonte_pequena = abrir_fonte(FONTE_TAMANHO_PEQ);
    }
    if (gui->fonte == NULL || gui->fonte_pequena == NULL) {
        gui_encerrar(gui);
        return false;
    }

    if (!criar_janela_principal(gui, titulo) || !criar_janela_secundaria(gui)) {
        gui_encerrar(gui);
        return false;
    }

    /* botões lado a lado na parte inferior da janela secundária */
    gui->botao_equalizar.rect = (SDL_FRect){ MARGEM, BOTAO_Y, BOTAO_LARGURA, BOTAO_ALTURA };
    gui->botao_equalizar.rotulo = "Equalizar";
    gui->botao_resolucao.rect = (SDL_FRect){ GUI_SECUNDARIA_LARGURA - MARGEM - BOTAO_LARGURA, BOTAO_Y,
                                             BOTAO_LARGURA, BOTAO_ALTURA };
    gui->botao_resolucao.rotulo = "Alterar resolução";

    return true;
}

void gui_encerrar(Gui *gui) {
    if (gui->textura_imagem)    { SDL_DestroyTexture(gui->textura_imagem);    gui->textura_imagem = NULL; }
    if (gui->render_secundario) { SDL_DestroyRenderer(gui->render_secundario); gui->render_secundario = NULL; }
    if (gui->janela_secundaria) { SDL_DestroyWindow(gui->janela_secundaria);   gui->janela_secundaria = NULL; }
    if (gui->render_principal)  { SDL_DestroyRenderer(gui->render_principal);  gui->render_principal = NULL; }
    if (gui->janela_principal)  { SDL_DestroyWindow(gui->janela_principal);    gui->janela_principal = NULL; }
    if (gui->fonte_pequena)     { TTF_CloseFont(gui->fonte_pequena);           gui->fonte_pequena = NULL; }
    if (gui->fonte)             { TTF_CloseFont(gui->fonte);                   gui->fonte = NULL; }
    if (TTF_WasInit()) {
        TTF_Quit();
    }
}

bool gui_definir_imagem(Gui *gui, SDL_Surface *imagem) {
    if (imagem == NULL) {
        return false;
    }
    if (gui->textura_imagem != NULL) {
        SDL_DestroyTexture(gui->textura_imagem);
        gui->textura_imagem = NULL;
    }

    gui->textura_imagem = SDL_CreateTextureFromSurface(gui->render_principal, imagem);
    if (gui->textura_imagem == NULL) {
        fprintf(stderr, "Erro ao criar textura da imagem: %s\n", SDL_GetError());
        return false;
    }
    gui->imagem_largura = imagem->w;
    gui->imagem_altura = imagem->h;
    return true;
}

bool gui_definir_resolucao(
    Gui *gui,
    int largura,
    int altura
) {
    SDL_DisplayID monitor = SDL_GetPrimaryDisplay();
    SDL_Rect limites_monitor;

    if (!SDL_GetDisplayBounds(
            monitor,
            &limites_monitor
        )) {

        fprintf(
            stderr,
            "Erro ao obter resolucao do monitor: %s\n",
            SDL_GetError()
        );

        return false;
    }

    if (!SDL_SetWindowSize(
            gui->janela_principal,
            largura,
            altura
        )) {

        fprintf(
            stderr,
            "Erro ao alterar a resolucao da janela: %s\n",
            SDL_GetError()
        );

        return false;
    }

    /*
     * Se a janela não cabe no monitor,
     * posiciona o canto superior esquerdo em (0,0).
     * Caso contrário, centraliza.
     */
    if (
        largura > limites_monitor.w ||
        altura > limites_monitor.h
    ) {

        SDL_SetWindowPosition(
            gui->janela_principal,
            0,
            0
        );

    } else {

        SDL_SetWindowPosition(
            gui->janela_principal,
            SDL_WINDOWPOS_CENTERED_DISPLAY(monitor),
            SDL_WINDOWPOS_CENTERED_DISPLAY(monitor)
        );
    }

    SDL_SyncWindow(gui->janela_principal);

    return true;
}

GuiAcao gui_processar_evento(Gui *gui, const SDL_Event *evento) {
    const SDL_WindowID id_secundaria = SDL_GetWindowID(gui->janela_secundaria);

    switch (evento->type) {
    case SDL_EVENT_QUIT:
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        return GUI_ACAO_SAIR;

    case SDL_EVENT_KEY_DOWN:
        if (evento->key.key == SDLK_ESCAPE) return GUI_ACAO_SAIR;
        if (evento->key.key == SDLK_S)      return GUI_ACAO_SALVAR;
        break;

    case SDL_EVENT_MOUSE_MOTION:
        if (evento->motion.windowID == id_secundaria) {
            gui->botao_equalizar.hover = botao_contem(&gui->botao_equalizar, evento->motion.x, evento->motion.y);
            gui->botao_resolucao.hover = botao_contem(&gui->botao_resolucao, evento->motion.x, evento->motion.y);
        }
        break;

    case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        if (evento->window.windowID == id_secundaria) {
            gui->botao_equalizar.hover = false;
            gui->botao_resolucao.hover = false;
            gui->botao_equalizar.pressionado = false;
            gui->botao_resolucao.pressionado = false;
        }
        break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (
            evento->button.windowID == id_secundaria &&
            evento->button.button == SDL_BUTTON_LEFT
        ) {

            gui->botao_equalizar.pressionado =
                botao_contem(
                    &gui->botao_equalizar,
                    evento->button.x,
                    evento->button.y
                );

            gui->botao_resolucao.pressionado =
                botao_contem(
                    &gui->botao_resolucao,
                    evento->button.x,
                    evento->button.y
                );
        }
        break;
    
    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (
            evento->button.windowID == id_secundaria &&
            evento->button.button == SDL_BUTTON_LEFT
        ) {

            bool clicou_equalizar =
                gui->botao_equalizar.pressionado &&
                botao_contem(
                    &gui->botao_equalizar,
                    evento->button.x,
                    evento->button.y
                );

            bool clicou_resolucao =
                gui->botao_resolucao.pressionado &&
                botao_contem(
                    &gui->botao_resolucao,
                    evento->button.x,
                    evento->button.y
                );

            gui->botao_equalizar.pressionado = false;
            gui->botao_resolucao.pressionado = false;

            if (clicou_equalizar) {
                return GUI_ACAO_EQUALIZAR;
            }

            if (clicou_resolucao) {
                return GUI_ACAO_ALTERAR_RESOLUCAO;
            }
        }
        break;
    }
    return GUI_ACAO_NENHUMA;
}

int gui_desenhar_texto(SDL_Renderer *render, TTF_Font *fonte,
                       const char *texto, float x, float y, SDL_Color cor) {
    SDL_Surface *superficie = TTF_RenderText_Blended(fonte, texto, 0, cor);
    if (superficie == NULL) {
        fprintf(stderr, "Erro ao renderizar texto '%s': %s\n", texto, SDL_GetError());
        return 0;
    }

    SDL_Texture *textura = SDL_CreateTextureFromSurface(render, superficie);
    const int largura = superficie->w;
    const SDL_FRect destino = { x, y, (float)superficie->w, (float)superficie->h };
    SDL_DestroySurface(superficie);

    if (textura == NULL) {
        fprintf(stderr, "Erro ao criar textura de texto: %s\n", SDL_GetError());
        return 0;
    }
    SDL_RenderTexture(render, textura, NULL, &destino);
    SDL_DestroyTexture(textura);
    return largura;
}

void gui_desenhar_principal(Gui *gui) {
    SDL_Renderer *render = gui->render_principal;
    int jw = 0, jh = 0;
    SDL_GetRenderOutputSize(render, &jw, &jh);

    definir_cor(render, COR_FUNDO_PRINCIPAL);
    SDL_RenderClear(render);

    if (gui->textura_imagem != NULL && gui->imagem_largura > 0 && gui->imagem_altura > 0) {
        /* ajusta a imagem à janela mantendo a proporção e centraliza */
        const float escala_x = (float)jw / (float)gui->imagem_largura;
        const float escala_y = (float)jh / (float)gui->imagem_altura;
        const float escala   = escala_x < escala_y ? escala_x : escala_y;

        SDL_FRect destino;
        destino.w = gui->imagem_largura * escala;
        destino.h = gui->imagem_altura * escala;
        destino.x = (jw - destino.w) / 2.0f;
        destino.y = (jh - destino.h) / 2.0f;

        SDL_RenderTexture(render, gui->textura_imagem, NULL, &destino);
    } else {
        gui_desenhar_texto(render, gui->fonte, "Nenhuma imagem carregada", MARGEM, MARGEM, COR_TEXTO);
    }

    SDL_RenderPresent(render);
}

/* Desenha as barras do histograma proporcionalmente ao maior valor. */
static void desenhar_histograma(Gui *gui, const Uint32 *histograma) {
    SDL_Renderer *render = gui->render_secundario;
    const SDL_FRect area = { HIST_X, HIST_Y, HIST_LARGURA, HIST_ALTURA };
    char rotulo[64];

    /* painel de fundo e linhas de grade horizontais (25%, 50%, 75%) */
    definir_cor(render, COR_PAINEL);
    SDL_RenderFillRect(render, &area);
    definir_cor(render, COR_GRADE);
    for (int i = 1; i < 4; ++i) {
        const float y = area.y + area.h * i / 4.0f;
        SDL_RenderLine(render, area.x, y, area.x + area.w, y);
    }

    Uint32 maximo = 0;
    if (histograma != NULL) {
        for (int i = 0; i < GUI_NIVEIS; ++i) {
            if (histograma[i] > maximo) maximo = histograma[i];
        }
    }

    if (maximo > 0) {
        SDL_FRect barras[GUI_NIVEIS];
        const float largura_barra = area.w / (float)GUI_NIVEIS;
        for (int i = 0; i < GUI_NIVEIS; ++i) {
            const float h = area.h * ((float)histograma[i] / (float)maximo);
            barras[i].x = area.x + i * largura_barra;
            barras[i].y = area.y + area.h - h;
            barras[i].w = largura_barra;
            barras[i].h = h;
        }
        definir_cor(render, COR_BARRA);
        SDL_RenderFillRects(render, barras, GUI_NIVEIS);
    }

    /* moldura por cima das barras */
    definir_cor(render, COR_GRADE);
    SDL_RenderRect(render, &area);

    /* rampa de cinza 0..255 abaixo do eixo x, para indicar a intensidade de cada barra */
    const float largura_nivel = area.w / (float)GUI_NIVEIS;
    for (int i = 0; i < GUI_NIVEIS; ++i) {
        const SDL_FRect faixa = { area.x + i * largura_nivel, area.y + area.h + 2, largura_nivel + 0.5f, HIST_RAMPA_H };
        SDL_SetRenderDrawColor(render, (Uint8)i, (Uint8)i, (Uint8)i, 255);
        SDL_RenderFillRect(render, &faixa);
    }

    /* rótulos dos eixos */
    const float y_rotulo = area.y + area.h + HIST_RAMPA_H + 6;
    gui_desenhar_texto(render, gui->fonte_pequena, "0", area.x, y_rotulo, COR_TEXTO_SUAVE);
    gui_desenhar_texto(render, gui->fonte_pequena, "128", area.x + area.w / 2 - 10, y_rotulo, COR_TEXTO_SUAVE);
    gui_desenhar_texto(render, gui->fonte_pequena, "255", area.x + area.w - 22, y_rotulo, COR_TEXTO_SUAVE);

    SDL_snprintf(rotulo, sizeof(rotulo), "máx: %u px", (unsigned)maximo);
    gui_desenhar_texto(render, gui->fonte_pequena, rotulo, area.x + 6, area.y + 4, COR_TEXTO_SUAVE);
}

void gui_desenhar_secundaria(Gui *gui, const GuiDadosSecundaria *dados) {
    SDL_Renderer *render = gui->render_secundario;

    definir_cor(render, COR_FUNDO_SECUNDARIA);
    SDL_RenderClear(render);

    gui_desenhar_texto(render, gui->fonte, "Histograma da imagem atual", MARGEM, 16, COR_TEXTO);
    desenhar_histograma(gui, dados != NULL ? dados->histograma : NULL);

    /* linhas de análise fornecidas pelo módulo de histograma */
    if (dados != NULL) {
        for (int i = 0; i < dados->num_linhas; ++i) {
            gui_desenhar_texto(render, gui->fonte, dados->linhas[i],
                               MARGEM, INFO_Y + i * INFO_ESPACO, COR_TEXTO);
        }
        gui->botao_equalizar.rotulo = dados->equalizada ? "Voltar ao original" : "Equalizar";
        gui->botao_resolucao.rotulo = dados->resolucao_original ? "1024x768" : "Resolucao original";
    }

    botao_desenhar(gui, &gui->botao_equalizar);
    botao_desenhar(gui, &gui->botao_resolucao);

    SDL_RenderPresent(render);
}
