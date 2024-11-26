#ifndef GAME__OBJECT__LIB__H
#define GAME__OBJECT__LIB__H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hitbox.h"
#include "texture.h"
#include "my_standard.h"

/* structure représentant un objet quelconque possédant des hitboxes et des sprites affichables */
typedef struct Game_object
{
    SDL_Texture *texture;
    int nb_row; // nb_row = nombre d'animations = nombre de hitboxes
    int nb_col; // nb_col = nombre de sprites par animation
    SDL_Rect src_rect, dst_rect;
    Hitbox *current_hitbox; // hitbox actuelle
    Hitbox **hitboxes; // tableau de hitboxes
}Game_object;

/* permet d'écrire les données d'un objet dans un fichier */
void save_Game_object(FILE **file, Game_object *object, const char *texture_file);

/* permet d'allouer un objet à partir de données présentes dans un fichier ; renvoie NULL en cas d'erreur */
Game_object *load_Game_object(FILE **file, SDL_Renderer *renderer);

/* permet de libérer la mémoire allouée pour un objet */
void destroy_Game_object(Game_object **object);

/* permet de déplacer un objet */
void move_Game_object(int x, int y, Game_object *object);

/* permet d'afficher un objet */
void display_Game_object(Game_object *object, SDL_Renderer *renderer);

/* permet de passer à l'image suivante de l'animation d'un objet */
void animate_Game_object(Game_object *object);

/* permet de changer l'animation en cours pour un objet */
void chg_animation_Game_object(int animation_line, Game_object *object);

/* renvoie SDL_TRUE si les deux objets sont en collision, SDL_FALSE sinon */
SDL_bool sat_Game_object(Game_object *object1, Game_object *object2);

#endif