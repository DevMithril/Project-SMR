#include "everything.h"

void loadGame(int id_save, Everything *all)
{
    load_Level(0, &all->level, all->renderer);
    load_Player(&all->player, all->renderer);
    load_Input(&all->input);
}

void runFrame(Everything *all)
{
    SDL_RenderClear(all->renderer);
    switch (all->gamemode)
    {
    case MAIN_MENU :
        loadGame(0, all);
        all->gamemode = IN_GAME;
        break;
    case IN_GAME :
        update_Game(&all->player, &all->input, &all->level, all->renderer);
        break;
    case PAUSE_MENU :
        break;
    default:
        break;
    };
    SDL_RenderPresent(all->renderer);
}

void quit(Everything *all, int status)
{
    /* liberation de la RAM allouee */

    destroy_Level(&all->level);
    destroy_Player(&all->player);

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
        quit(all, EXIT_FAILURE);
    }
    if (TTF_Init())
    {
        fprintf(stderr, "Erreur TTF_Init : %s\n", TTF_GetError());
        quit(all, EXIT_FAILURE);
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
        quit(all, EXIT_FAILURE);
    }
    all->renderer = SDL_CreateRenderer(all->window, -1, SDL_RENDERER_ACCELERATED);
    if (NULL == all->renderer)
    {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s\n", SDL_GetError());
        quit(all, EXIT_FAILURE);
    }
    SDL_RenderSetLogicalSize(all->renderer, res_x, res_y);

    /* chargement et affichage de l'icone de la fenetre */
    
    if (!strcmp(icon_path, "none"))
    {
        return;
    }
    SDL_Surface *icone;
    icone = SDL_LoadBMP(icon_path);
    if (NULL == icone)
    {
        fprintf(stderr, "Erreur SDL_LoadBMP : %s\n", SDL_GetError());
        quit(all, EXIT_FAILURE);
    }
    SDL_SetWindowIcon(all->window, icone);
    SDL_FreeSurface(icone);
}