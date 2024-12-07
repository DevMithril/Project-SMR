#ifndef HITBOX__LIB__H
#define HITBOX__LIB__H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Hitbox
{
    int nb_points;
    SDL_Point *points; // les coordonnées des points sont exprimées par rapport au centre du cercle
    int cercle_x, cercle_y, cercle_rayon; // le cercle contenant toute la hitbox (permet optimisation dans la fonction sat)
}Hitbox;

/* renvoie SDL_TRUE si les deux hitboxes sont en collision, SDL_FALSE sinon */
SDL_bool sat(Hitbox *hitbox1, Hitbox *hitbox2);

/* permet d'écrire les données d'une hitbox dans un fichier */
void save_Hitbox(FILE **file, Hitbox *hitbox);

/* permet d'allouer une hitbox à partir de données présentes dans un fichier ; renvoie NULL en cas d'erreur */
Hitbox *load_Hitbox(FILE **file);

/* permet de libérer la mémoire allouée pour une hitbox */
void destroy_Hitbox(Hitbox **hitbox);

/* permet de déplacer une hitbox */
void move_Hitbox(int x, int y, Hitbox *hitbox);

/* permet d'afficher une hitbox */
void display_Hitbox(Hitbox *hitbox, SDL_Renderer *renderer);

#endif