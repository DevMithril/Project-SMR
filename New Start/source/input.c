#include "input.h"

void updateEvent(Input *input)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
            input->quit = SDL_TRUE;
        else if(event.type == SDL_KEYDOWN)
        {
            input->key[event.key.keysym.scancode] = SDL_TRUE;
        }
        else if(event.type == SDL_KEYUP)
            input->key[event.key.keysym.scancode] = SDL_FALSE;
    }
    input->down = input->key[SDL_GetScancodeFromKey(input->key_down)];
    input->left = input->key[SDL_GetScancodeFromKey(input->key_left)];
    input->right = input->key[SDL_GetScancodeFromKey(input->key_right)];
    input->up = input->key[SDL_GetScancodeFromKey(input->key_up)];
}

void resetKeyState(SDL_Keycode key, Input *input)
{
    input->key[SDL_GetScancodeFromKey(key)] = SDL_FALSE;
}