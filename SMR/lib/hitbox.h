#ifndef HITBOX__LIB__H
#define HITBOX__LIB__H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NB_POINTS 4

typedef struct Vector2
{
    double x, y;
}Vector2;

typedef struct Hitbox
{
    SDL_Point points[NB_POINTS];
}Hitbox;

/* renvoie SDL_TRUE si les deux hitboxes sont en collision, SDL_FALSE sinon */
Vector2 sat(Hitbox *hitbox1, Hitbox *hitbox2);

/* permet de déplacer une hitbox */
void move_Hitbox(int x, int y, Hitbox *hitbox);

/* permet d'afficher une hitbox */
void display_Hitbox(Hitbox *hitbox, int offset_x, int offset_y, SDL_Renderer *renderer);

#endif