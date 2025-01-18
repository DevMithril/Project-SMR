#ifndef PLAYER__LIB__H
#define PLAYER__LIB__H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "hitbox.h"
#include "level.h"
#include "input.h"
#include "texture.h"

typedef struct Player
{
    Hitbox hitbox;
    SDL_Rect src, dst;
    SDL_Texture *texture;
    int nb_row; // nb_row = nombre d'animations
    int nb_col; // nb_col = nombre de sprites par animation
}Player;

/* permet d'allouer le player à partir de données présentes dans un fichier ; renvoie NULL en cas d'erreur */
void load_Player(Player *player, SDL_Renderer *renderer);

/* permet de libérer la mémoire allouée pour le player */
void destroy_Player(Player *player);

/* permet de déplacer le joueur */
void move_Player(int x, int y, Player *player, Level *level);

/* permet d'afficher le joueur */
void display_Player(Player *player, SDL_Renderer *renderer);

/* permet de passer à l'image suivante de l'animation du joueur */
void animate_Player(Player *player);

/* permet de changer l'animation en cours du joueur */
void chg_animation_Player(int animation_line, Player *player);

/* met à jour le joueur, execute une frame pour le joueur */
void update_Game(Player *player, Input *input, Level *level, SDL_Renderer *renderer);

#endif