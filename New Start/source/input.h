#ifndef INPUT__LIB__H
#define INPUT__LIB__H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Input
{
    SDL_bool key[SDL_NUM_SCANCODES];
    SDL_bool quit;
    SDL_Scancode key_up, key_down, key_left, key_right;
    SDL_bool up, down, left, right;
}Input;

/* met à jour la structure input */
void update_Input(Input *input);

/* force une key à SDL_FALSE */
void resetKeyState_Input(SDL_Scancode key, Input *input);

#endif