#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

typedef struct Input
{
    SDL_bool key[SDL_NUM_SCANCODES];
    SDL_bool quit;
    SDL_Keycode key_up, key_down, key_left, key_right, key_Validate, key_Summit, key_Delete, key_Help;  /* /;8;7;9;Space;Return;D;H */
    SDL_bool up, down, left, right, Validate, Summit, Delete, Help;
}Input;

typedef struct Data
{
    ;
}Data;

typedef struct Everything
{
    Input input;
    Data data;
    SDL_Renderer *renderer;
    SDL_Window *window;
}Everything;

void updateEvent(Input *input)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
            input->quit = SDL_TRUE;
        else if(event.type == SDL_KEYDOWN)
        {
            input->key[event.key.keysym.scancode] = SDL_TRUE;
        }
        else if(event.type == SDL_KEYUP)
            input->key[event.key.keysym.scancode] = SDL_FALSE;
    }
    input->Validate = input->key[SDL_GetScancodeFromKey(input->key_Validate)];
    input->Summit = input->key[SDL_GetScancodeFromKey(input->key_Summit)];
    input->Delete = input->key[SDL_GetScancodeFromKey(input->key_Delete)];
    input->Help = input->key[SDL_GetScancodeFromKey(input->key_Help)];
    input->down = input->key[SDL_GetScancodeFromKey(input->key_down)];
    input->left = input->key[SDL_GetScancodeFromKey(input->key_left)];
    input->right = input->key[SDL_GetScancodeFromKey(input->key_right)];
    input->up = input->key[SDL_GetScancodeFromKey(input->key_up)];
}

void quit(Everything *all, int status)
{
    /* liberation de la RAM allouee */

    /* destruction du renderer et de la fenetre, fermetures de la TTF et de la SDL puis sortie du programme */

    if (NULL != all->renderer)
        SDL_DestroyRenderer(all->renderer);
    if (NULL != all->window)
        SDL_DestroyWindow(all->window);
    TTF_Quit();
    SDL_Quit();
    printf("program returns code : %d\n", status);
    exit(status);
}

void init(Everything *all, const char *window_name, const char *icon_path, int res_x, int res_y, SDL_bool fullscreen)
{
    /* initialisation de la SDL et de la TTF */

    if (SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "Erreur SDL_Init : %s\n", SDL_GetError());
        Quit(all, EXIT_FAILURE);
    }
    if (TTF_Init())
    {
        fprintf(stderr, "Erreur TTF_Init : %s\n", TTF_GetError());
        Quit(all, EXIT_FAILURE);
    }

    /* creation de la fenetre et du renderer associé */

    if (fullscreen)
    {
        all->window = SDL_CreateWindow(window_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                10*res_x, 10*res_y, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
    }
    else
    {
        all->window = SDL_CreateWindow(window_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                4*res_x, 4*res_y, SDL_WINDOW_SHOWN);
    }
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
    SDL_RenderSetLogicalSize(all->renderer, res_x, res_y);

    /* chargement et affichage de l'icone de la fenetre */
    
    if (strcmp(icon_path, ""))
    {
        return;
    }
    SDL_Surface *icone;
    icone = SDL_LoadBMP(icon_path);
    if (NULL == icone)
    {
        fprintf(stderr, "Erreur SDL_LoadBMP : %s\n", SDL_GetError());
        Quit(all, EXIT_FAILURE);
    }
    SDL_SetWindowIcon(all->window, icone);
    SDL_FreeSurface(icone);
}

int main(int argc, char *argv[])
{
    ;
}