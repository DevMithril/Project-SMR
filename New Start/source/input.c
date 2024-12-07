#include "input.h"

void update_Input(Input *input)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT :
            input->quit = SDL_TRUE;
            break;
        case SDL_KEYDOWN :
            input->key[event.key.keysym.scancode] = SDL_TRUE;
            break;
        case SDL_KEYUP :
            input->key[event.key.keysym.scancode] = SDL_FALSE;
            break;
        }
    }
    input->down = input->key[input->key_down];
    input->left = input->key[input->key_left];
    input->right = input->key[input->key_right];
    input->up = input->key[input->key_up];
}

void resetKeyState_Input(SDL_Scancode key, Input *input)
{
    input->key[key] = SDL_FALSE;
}