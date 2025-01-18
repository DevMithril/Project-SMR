#ifndef LEVEL__LIB__H
#define LEVEL__LIB__H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "hitbox.h"
#include "texture.h"

#define NB_TILEMAPS 1

typedef struct Tile
{
    SDL_Rect src, dst;
    int tilemap_id;
}Tile;

typedef struct Level
{
    SDL_Rect src;
    SDL_Texture *texture;
    int size_x, size_y;
    int nb_hitboxes;
    Hitbox *hitboxes;
}Level;

/* renvoie SDL_TRUE si la hitbox est en collision avec le level, SDL_FALSE sinon */
SDL_bool collision_Level(Hitbox *hitbox, Level *level);

/* charge les tilemaps */
SDL_Texture **load_Tilemaps(SDL_Renderer *renderer);

/* libère la mémoire allouée pour les tilemaps */
void destroy_Tilemaps(SDL_Texture **tilemaps);

/* permet d'allouer un level à partir de son id ; renvoie NULL en cas d'erreur */
void load_Level(int level_id, Level *level, SDL_Renderer *renderer);

/* permet de libérer la mémoire allouée pour un level */
void destroy_Level(Level *level);

/* permet de déplacer la caméra */
void move_cam_Level(int x, int y, SDL_bool *bool_x, SDL_bool *bool_y, Level *level);

/* permet d'afficher un level */
void display_Level(Level *level, SDL_Renderer *renderer);

#endif