#include "game.h"

void initGame(Everything *all)
{
    load_Level(NULL, &all->level, all->renderer);
}

void runGame(Everything *all)
{
    SDL_RenderClear(all->renderer);
    switch (all->gamemode)
    {
    case MAIN_MENU :
        break;
    case IN_GAME :
        display_Level(&all->level, all->renderer);
        break;
    case PAUSE_MENU :
        break;
    default:
        break;
    };
    SDL_RenderPresent(all->renderer);
}