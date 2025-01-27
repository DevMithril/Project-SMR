#ifndef INPUT__LIB__H
#define INPUT__LIB__H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Input
{
    SDL_bool key[SDL_NUM_SCANCODES];
    SDL_bool quit;
    SDL_Scancode key_up, key_down, key_left, key_right, key_switch, key_tilemap_add, key_tilemap_sub, key_erase, key_draw, key_save, key_toggle_hitbox;
    SDL_bool up, down, left, right, switch_, tilemap_add, tilemap_sub, erase, draw, save, toggle_hitbox;
}Input;

/* charge les scancodes des touches de control */
void loadKeys(Input *input);

/* met à jour la structure input */
void update_Input(Input *input);

/* force une key à SDL_FALSE */
void resetKeyState_Input(SDL_Scancode key, Input *input);

#endif