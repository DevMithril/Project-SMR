#ifndef EVERYTHING__LIB__H
#define EVERYTHING__LIB__H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "input.h"
#include "level.h"

typedef enum Gamemode
{
    MAIN_MENU,
    IN_GAME,
    PAUSE_MENU
}Gamemode;

typedef struct Context
{
    int last_save_id;
    int current_room_id;
}Context;

typedef struct Everything
{
    Context context;
    Level level;
    Input input;
    Gamemode gamemode;
    SDL_Renderer *renderer;
    SDL_Window *window;
}Everything;

/* quitte le programme après avoir libéré la mémoire allouée */
void quit(Everything *all, int status);

/* initialise la SDL et la TTF, crée la fenetre et le renderer associé */
void init(Everything *all, const char *window_name, const char *icon_path, int res_x, int res_y, SDL_bool fullscreen);

/* execute une frame du jeu */
void runFrame(Everything *all);

/* permet de charger une partie depuis une sauvegarde */
void loadGame(int id_save, Everything *all);

#endif