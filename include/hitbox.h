#ifndef HITBOX__LIB__H
#define HITBOX__LIB__H
#include <SDL2/SDL.h>

typedef struct Hitbox
{
    int nb_points;
    SDL_Point *points;
    int cercle_x, cercle_y, cercle_rayon;
}Hitbox;

/* renvoie SDL_TRUE si les deux hitboxes sont en collision, SDL_FALSE sinon */
SDL_bool sat(Hitbox *hitbox1, Hitbox *hitbox2);

/* permet de remplir une hitbox à partir de données présentes dans un fichier ; toutes les données seront nulles en cas d'erreur */
void load_Hitbox(FILE *file, Hitbox *hitbox);

/* permet de vider une hitbox de ses données */
void destroy_Hitbox(Hitbox *hitbox);

/* permet de déplacer une hitbox */
void move_Hitbox(int x, int y, Hitbox *hitbox);

#endif