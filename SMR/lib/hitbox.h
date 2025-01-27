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

/* renvoie le vecteur de transition minimale resolvant l'éventuelle collision entre les deux hitboxes */
Vector2 sat(Hitbox *hitbox1, Hitbox *hitbox2);

/* renvoie le vecteur de transition minimale (MTV) resolvant l'éventuelle collision entre la hitbox et la liste de hitboxes */
Vector2 sat_bulk(Hitbox *hitbox, Hitbox *hitbox_array, int array_range);

/* permet de déplacer une hitbox */
void move_Hitbox(int x, int y, Hitbox *hitbox);

/* permet d'afficher une hitbox */
void display_Hitbox(Hitbox *hitbox, int offset_x, int offset_y, int r, int g, int b, SDL_Renderer *renderer);

#endif