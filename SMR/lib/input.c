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
    input->jump = input->key[input->key_jump];
}

void resetKeyState_Input(SDL_Scancode key, Input *input)
{
    input->key[key] = SDL_FALSE;
}

void load_Input(Input *input)
{
    input->key_down = SDL_GetScancodeFromKey(SDLK_j);
    input->key_up = SDL_GetScancodeFromKey(SDLK_u);
    input->key_right = SDL_GetScancodeFromKey(SDLK_k);
    input->key_left = SDL_GetScancodeFromKey(SDLK_h);
    input->key_jump = SDL_GetScancodeFromKey(SDLK_SPACE);
}