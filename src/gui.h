#ifndef GUI_H
#define GUI_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

/*
 * Interface gráfica do projeto (Luis Fernando).
 *
 * - Janela principal: exibe a imagem em processamento. Inicia em 1024x768,
 *   centralizada no monitor principal.
 * - Janela secundária: criada como filha da janela principal, posicionada em
 *   (0,0). Exibe o histograma da imagem atual, as informações de análise e
 *   os botões "Equalizar"/"Voltar ao original" e "Alterar resolução".
 * - Todos os textos são desenhados com SDL_ttf usando a fonte DejaVu Sans,
 *   distribuída junto com o projeto em assets/fonts/.
 */

#define GUI_NIVEIS 256   /* quantidade de níveis de cinza do histograma */

enum {
    GUI_PRINCIPAL_LARGURA   = 1024, /* resolução inicial da janela principal */
    GUI_PRINCIPAL_ALTURA    = 768,
    GUI_SECUNDARIA_LARGURA  = 560,
    GUI_SECUNDARIA_ALTURA   = 470,
};

/* Ações geradas pela interface a partir dos eventos do usuário. */
typedef enum {
    GUI_ACAO_NENHUMA = 0,
    GUI_ACAO_SAIR,               /* fechar qualquer janela, ESC ou SDL_EVENT_QUIT */
    GUI_ACAO_EQUALIZAR,          /* botão "Equalizar" / "Voltar ao original" */
    GUI_ACAO_ALTERAR_RESOLUCAO,  /* botão "Alterar resolução" */
    GUI_ACAO_SALVAR              /* tecla S */
} GuiAcao;

typedef struct {
    SDL_FRect   rect;
    const char *rotulo;
    bool        hover;   /* mouse sobre o botão (feedback visual) */
} GuiBotao;

typedef struct {
    SDL_Window   *janela_principal;
    SDL_Renderer *render_principal;
    SDL_Window   *janela_secundaria;
    SDL_Renderer *render_secundario;

    TTF_Font *fonte;          /* fonte padrão (títulos, botões) */
    TTF_Font *fonte_pequena;  /* rótulos de eixo e informações */

    SDL_Texture *textura_imagem;   /* imagem atual, pronta para exibição */
    int          imagem_largura;
    int          imagem_altura;

    GuiBotao botao_equalizar;
    GuiBotao botao_resolucao;
} Gui;

/*
 * Dados exibidos na janela secundária. Devem ser preenchidos pelo módulo de
 * análise do histograma (a GUI apenas desenha o que receber).
 */
typedef struct {
    const Uint32       *histograma;   /* vetor com GUI_NIVEIS contagens */
    const char *const  *linhas;       /* linhas de texto de análise (média, desvio etc.) */
    int                 num_linhas;
    bool                equalizada;   /* true: botão mostra "Voltar ao original" */
} GuiDadosSecundaria;

/* Cria as duas janelas, os renderizadores e carrega a fonte.
 * Retorna false (com mensagem no stderr) se qualquer etapa falhar. */
bool gui_iniciar(Gui *gui, const char *titulo);

/* Libera todos os recursos criados por gui_iniciar. Seguro chamar mais de uma vez. */
void gui_encerrar(Gui *gui);

/* Converte a surface em textura para exibição na janela principal.
 * Deve ser chamada sempre que a imagem atual mudar (ex.: após equalizar). */
bool gui_definir_imagem(Gui *gui, SDL_Surface *imagem);

/* Redimensiona a janela principal e a recentraliza no monitor principal. */
bool gui_definir_resolucao(Gui *gui, int largura, int altura);

/* Traduz um evento SDL em uma ação da interface (cliques nos botões, teclas, fechar). */
GuiAcao gui_processar_evento(Gui *gui, const SDL_Event *evento);

/* Desenha a janela principal (imagem ajustada e centralizada). */
void gui_desenhar_principal(Gui *gui);

/* Desenha a janela secundária (histograma, análise e botões). */
void gui_desenhar_secundaria(Gui *gui, const GuiDadosSecundaria *dados);

/* Desenha uma linha de texto em (x, y) com a fonte e cor informadas.
 * Retorna a largura do texto desenhado em pixels (0 em caso de erro). */
int gui_desenhar_texto(SDL_Renderer *render, TTF_Font *fonte,
                       const char *texto, float x, float y, SDL_Color cor);

#endif
