#ifndef HITBOX__LIB__H
#define HITBOX__LIB__H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Hitbox
{
    int nb_points;
    SDL_Point *points; // les coordonnées des points sont exprimées par rapport au centre du cercle
    int cercle_x, cercle_y, cercle_rayon; // le cercle contenant toute la hitbox (sert de radar)
}Hitbox;

/* renvoie SDL_TRUE si les deux hitboxes sont en collision, SDL_FALSE sinon */
SDL_bool sat(Hitbox *hitbox1, Hitbox *hitbox2);

#endif