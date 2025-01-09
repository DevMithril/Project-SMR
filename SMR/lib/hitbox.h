#ifndef HITBOX__LIB__H
#define HITBOX__LIB__H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define NB_POINTS 4

typedef struct Hitbox
{
    SDL_Point points[NB_POINTS];
}Hitbox;

/* renvoie SDL_TRUE si les deux hitboxes sont en collision, SDL_FALSE sinon */
SDL_bool sat(Hitbox *hitbox1, Hitbox *hitbox2);

/* permet de déplacer une hitbox */
void move_Hitbox(int x, int y, Hitbox *hitbox);

/* permet d'afficher une hitbox */
void display_Hitbox(Hitbox *hitbox, SDL_Renderer *renderer);

#endif