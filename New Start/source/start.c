#include <SDL2/SDL_ttf.h>
#include <string.h>
#include "everything.h"
#include "input.h"

int main(int argc, char *argv[])
{
    /* Création des variables */

    Everything all = {.renderer = NULL, .window = NULL};
    all.input.quit = SDL_FALSE;
    all.mode = 0;
    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
        all.input.key[i] = SDL_FALSE;

    /* Initialisations, création de la fenêtre et du renderer. */

    init(&all, "Name", "", 320, 240, SDL_TRUE);

    /* Boucle principale du jeu */

    while (!all.input.quit)
    {
        update_Input(&all.input);
        runGame(&all);
        SDL_Delay((int)(1000 / 60));
    }

    /* Fermeture du logiciel et libération de la mémoire */

    quit(&all, EXIT_SUCCESS);
}