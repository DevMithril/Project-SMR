#ifndef LEVEL__LIB__H
#define LEVEL__LIB__H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "hitbox.h"
#include "texture.h"

typedef struct Level
{
    SDL_Rect src;
    SDL_Texture *texture;
    int nb_hitboxes;
    Hitbox *hitboxes;
}Level;

/* renvoie SDL_TRUE si la hitbox est en collision avec le level, SDL_FALSE sinon */
SDL_bool collision_Level(Hitbox *hitbox, Level *level);

/* permet d'allouer un level à partir de données présentes dans un fichier ; renvoie NULL en cas d'erreur */
void load_Level(FILE **file, Level *level, SDL_Renderer *renderer);

/* permet de libérer la mémoire allouée pour un level */
void destroy_Level(Level *level);

/* permet de déplacer la caméra */
void move_cam_Level(int x, int y, Level *level);

/* permet d'afficher un level */
void display_Level(Level *level, SDL_Renderer *renderer);

#endif