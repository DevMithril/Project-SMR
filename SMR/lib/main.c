#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "everything.h"

int main(int argc, char *argv[])
{
    /* Création des variables */

    Everything all = {.renderer = NULL, .window = NULL, .gamemode = MAIN_MENU, .input.quit = SDL_FALSE};
    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
        all.input.key[i] = SDL_FALSE;

    /* Initialisations, création de la fenêtre et du renderer. */

    init(&all, "SMR", "none", 320, 240, SDL_FALSE);

    /* Boucle principale du jeu */

    while (!all.input.quit)
    {
        update_Input(&all.input);
        runFrame(&all);
        SDL_Delay((int)(1000 / 60));
    }

    /* Fermeture du logiciel et libération de la mémoire */

    quit(&all, EXIT_SUCCESS);
}