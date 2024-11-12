#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/hitbox.h"
#include "include/texture.h"
#include "include/game_object.h"

/* 
* resolution : 320x240
* ligne de commande pour la compilation : gcc -o Brick_Breaker Brick_Breaker.c -lm $(sdl2-config --cflags --libs) -l SDL2_ttf
* pour utiliser valgrind (memoire) : 
*   valgrind -s --tool=memcheck --leak-check=yes|no|full|summary --leak-resolution=low|med|high --show-reachable=yes ./Brick_Breaker
*/

typedef struct Text
{
    int x, y;
    SDL_Texture *texture;
    SDL_Rect src_rect, dst_rect;
    struct Text *suivant;
}Text;

typedef struct Fonts
{
    TTF_Font *titles, *menu_button, *secondary_titles;
    SDL_Color rouge, vert, vert_clair;
}Fonts;

typedef struct Input
{
    SDL_bool key[SDL_NUM_SCANCODES];
    SDL_bool quit;
    SDL_KeyCode wanted_input;
    SDL_Keycode key_up, key_down, key_left, key_right, key_L, key_R, key_start, key_select, key_A, key_B;  /* /;8;7;9;A;Z;Return;E;Space;D */
    SDL_bool up, down, left, right, L, R, start, select, A, B;
    SDL_bool select_on_cooldown, start_on_cooldown, waiting_for_input;
}Input;

typedef struct Everything
{
    Text liste_text;
    Fonts fonts;
    int game_state;
    Input input;
    SDL_Renderer *renderer;
    SDL_Window *window;
}Everything;

void updateEvent(Input *input)
{
    SDL_Event event;
    input->wanted_input = -1;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
            input->quit = SDL_TRUE;
        else if(event.type == SDL_KEYDOWN)
        {
            if (input->waiting_for_input)
            {
                input->wanted_input = SDL_GetKeyFromScancode(event.key.keysym.scancode);
            }
            input->key[event.key.keysym.scancode] = SDL_TRUE;
        }
        else if(event.type == SDL_KEYUP)
            input->key[event.key.keysym.scancode] = SDL_FALSE;
    }
    input->A = input->key[SDL_GetScancodeFromKey(input->key_A)];
    input->B = input->key[SDL_GetScancodeFromKey(input->key_B)];
    input->down = input->key[SDL_GetScancodeFromKey(input->key_down)];
    input->L = input->key[SDL_GetScancodeFromKey(input->key_L)];
    input->left = input->key[SDL_GetScancodeFromKey(input->key_left)];
    input->R = input->key[SDL_GetScancodeFromKey(input->key_R)];
    input->right = input->key[SDL_GetScancodeFromKey(input->key_right)];
    input->up = input->key[SDL_GetScancodeFromKey(input->key_up)];
    if (input->select)
    {
        if (input->key[SDL_GetScancodeFromKey(input->key_select)])
        {
            input->select_on_cooldown = SDL_TRUE;
        }
        input->select = SDL_FALSE;
    }
    else if (input->select_on_cooldown)
    {
        if (!input->key[SDL_GetScancodeFromKey(input->key_select)])
        {
            input->select_on_cooldown = SDL_FALSE;
        }
    }
    else
    {
        input->select = input->key[SDL_GetScancodeFromKey(input->key_select)];
    }

    if (input->start)
    {
        if (input->key[SDL_GetScancodeFromKey(input->key_start)])
        {
            input->start_on_cooldown = SDL_TRUE;
        }
        input->start = SDL_FALSE;
    }
    else if (input->start_on_cooldown)
    {
        if (!input->key[SDL_GetScancodeFromKey(input->key_start)])
        {
            input->start_on_cooldown = SDL_FALSE;
        }
    }
    else
    {
        input->start = input->key[SDL_GetScancodeFromKey(input->key_start)];
    }
}

Text *loadText(TTF_Font *font, const char text[], SDL_Color color, Everything *all)
{
    SDL_Surface *surface = NULL; 
    SDL_Texture *texture = NULL, *tmp = NULL;
    Text *last = &all->liste_text;
    while (last->suivant != NULL)
    {
        last = last->suivant;
    }
    surface = TTF_RenderText_Solid(font, text, color);
    if(NULL == surface)
    {
        fprintf(stderr, "Erreur TTF_RenderText_Solid : %s\n", TTF_GetError());
        return NULL;
    }
    tmp = SDL_CreateTextureFromSurface(all->renderer, surface);
    if(NULL == tmp)
    {
        fprintf(stderr, "Erreur SDL_CreateTextureFromSurface : %s\n", SDL_GetError());
        return NULL;
    }
    texture = SDL_CreateTexture(all->renderer, SDL_PIXELFORMAT_RGBA8888, 
                            SDL_TEXTUREACCESS_TARGET, surface->w, surface->h);
    if(NULL == texture)
    {
        fprintf(stderr, "Erreur SDL_CreateTexture : %s\n", SDL_GetError());
        return NULL;
    }

    last->suivant = malloc(sizeof(Text));
    last->suivant->texture = texture;
    last->suivant->src_rect.h = surface->h;
    last->suivant->src_rect.w = surface->w;
    last->suivant->src_rect.x = 0;
    last->suivant->src_rect.y = 0;
    last->suivant->dst_rect.h = surface->h;
    last->suivant->dst_rect.w = surface->w;
    last->suivant->dst_rect.x = 0;
    last->suivant->dst_rect.y = 0;
    last->suivant->x = 0;
    last->suivant->y = 0;
    last->suivant->suivant = NULL;

    SDL_SetRenderTarget(all->renderer, texture);
    SDL_RenderCopy(all->renderer, tmp, NULL, NULL);
    SDL_DestroyTexture(tmp);
    SDL_FreeSurface(surface);
    SDL_SetRenderTarget(all->renderer, NULL);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return last->suivant;
}

void destroyText(Text *text, Everything *all)
{
    Text *text_liste = &all->liste_text, *tmp = NULL;
    if (text == NULL)
    {
        fprintf(stderr, "Erreur dans destroyText : le Text passé en paramètre est un pointeur NULL\n");
        return;
    }
    while (text_liste->suivant != text)
    {
        text_liste = text_liste->suivant;
    }
    if (text_liste->suivant->texture != NULL)
    {
        SDL_DestroyTexture(text_liste->suivant->texture);
    }
    tmp = text_liste->suivant->suivant;
    free(text_liste->suivant);
    text_liste->suivant = tmp;
}

void destroyFonts(Everything *all)
{
    Fonts *fonts = &all->fonts;
    if (fonts->titles != NULL)
    {
        TTF_CloseFont(fonts->titles);
    }
    if (fonts->menu_button != NULL)
    {
        TTF_CloseFont(fonts->menu_button);
    }
    if (fonts->secondary_titles != NULL)
    {
        TTF_CloseFont(fonts->secondary_titles);
    }
}

void loadFonts(Everything *all)
{
    Fonts *fonts = &all->fonts;
    fonts->titles = TTF_OpenFont("data/fonts/8-bitanco.ttf", 30);
    if (fonts->titles == NULL)
    {
        fprintf(stderr, "Erreur TTF_OpenFont : %s\n", TTF_GetError());
    }
    fonts->menu_button = TTF_OpenFont("data/fonts/alagard.ttf", 18);
    if (fonts->menu_button == NULL)
    {
        fprintf(stderr, "Erreur TTF_OpenFont : %s\n", TTF_GetError());
    }
    fonts->secondary_titles = TTF_OpenFont("data/fonts/upheavtt.ttf", 30);
    if (fonts->secondary_titles == NULL)
    {
        fprintf(stderr, "Erreur TTF_OpenFont : %s\n", TTF_GetError());
    }
    SDL_Color rouge = {200, 0, 0};
    SDL_Color vert = {0, 200, 0};
    SDL_Color vert_clair = {140, 200, 140};
    fonts->rouge = rouge;
    fonts->vert = vert;
    fonts->vert_clair = vert_clair;
}

void Quit(Everything *all, int status)
{
    /* liberation de la RAM allouee */

    while (all->liste_text.suivant != NULL)
    {
        destroyText(all->liste_text.suivant, all);
    }
    destroyFonts(all);

    /* destruction du renderer et de la fenetre, fermeture de la SDL puis sortie du programme */

    if (NULL != all->renderer)
        SDL_DestroyRenderer(all->renderer);
    if (NULL != all->window)
        SDL_DestroyWindow(all->window);
    TTF_Quit();
    SDL_Quit();
    if (status == EXIT_SUCCESS)
        printf("L'exécution du programme s'est bien terminée.\n");
    else
        printf("L'exécution du programme s'est soldée par une erreur.\n");
    exit(status);
}

void Init(Everything *all)
{
    /* initialisation de la SDL et de la TTF */

    if (0 != SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "Erreur SDL_Init : %s\n", SDL_GetError());
        Quit(all, EXIT_FAILURE);
    }
    if (0 != TTF_Init())
    {
        fprintf(stderr, "Erreur TTF_Init : %s\n", TTF_GetError());
        Quit(all, EXIT_FAILURE);
    }

    /* creation de la fenetre et du renderer associé */

    all->window = SDL_CreateWindow("Brick Breaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              4*320, 4*240, SDL_WINDOW_SHOWN /*| SDL_WINDOW_FULLSCREEN*/);
    if (NULL == all->window)
    {
        fprintf(stderr, "Erreur SDL_CreateWindow : %s\n", SDL_GetError());
        Quit(all, EXIT_FAILURE);
    }
    all->renderer = SDL_CreateRenderer(all->window, -1, SDL_RENDERER_ACCELERATED);
    if (NULL == all->renderer)
    {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s\n", SDL_GetError());
        Quit(all, EXIT_FAILURE);
    }
    SDL_RenderSetLogicalSize(all->renderer, 320, 240);

    /* chargement et affichage de l'icone de la fenetre */

    SDL_Surface *icone;
    icone = SDL_LoadBMP("icone.bmp");
    if (NULL == icone)
    {
        fprintf(stderr, "Erreur SDL_LoadBMP : %s\n", SDL_GetError());
        Quit(all, EXIT_FAILURE);
    }
    SDL_SetWindowIcon(all->window, icone);
    SDL_FreeSurface(icone);
}

void updateGameState(Everything *all)
{
    return;
}

void runGame(Everything *all)
{
    SDL_RenderClear(all->renderer);

    switch (all->game_state)
    {   
        case 0 :        /*  */
        {
            break;
        };
    }
    updateGameState(all);

    SDL_RenderPresent(all->renderer);
}

int main(int argc, char *argv[])
{
    /* Création des variables */

    Everything all = {.renderer = NULL, .window = NULL};
    all.input.quit = SDL_FALSE;
    all.game_state = 0;
    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
        all.input.key[i] = SDL_FALSE;

    /* Initialisation, création de la fenêtre et du renderer. */

    Init(&all);

    /* Chargement des options et du level */

    loadFonts(&all);

    /* Boucle principale du jeu */

    while (!all.input.quit)
    {
        updateEvent(&all.input);
        runGame(&all);
        SDL_Delay( (int)(1000 / 60) );
    }

    /* Fermeture du logiciel et libération de la mémoire */

    Quit(&all, EXIT_SUCCESS);
}