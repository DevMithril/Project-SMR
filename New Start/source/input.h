#ifndef INPUT__LIB__H
#define INPUT__LIB__H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Input
{
    SDL_bool key[SDL_NUM_SCANCODES];
    SDL_bool quit;
    SDL_Keycode key_up, key_down, key_left, key_right;  /* arrows */
    SDL_bool up, down, left, right;
}Input;

/* met à jour la structure input */
void updateEvent(Input *input);

/* force l'input à SDL_FALSE */
void resetKeyState(SDL_Keycode key, Input *input);

#endif