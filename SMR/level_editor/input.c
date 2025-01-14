#include "input.h"

void loadKeys(Input *input)
{
    input->key_down = SDL_GetScancodeFromKey(SDLK_j);
    input->key_up = SDL_GetScancodeFromKey(SDLK_u);
    input->key_right = SDL_GetScancodeFromKey(SDLK_k);
    input->key_left = SDL_GetScancodeFromKey(SDLK_h);
    input->key_switch_cursor = SDL_GetScancodeFromKey(SDLK_a);
    input->key_tilemap_add = SDL_GetScancodeFromKey(SDLK_r);
    input->key_tilemap_sub = SDL_GetScancodeFromKey(SDLK_e);
    input->key_erase = SDL_GetScancodeFromKey(SDLK_z);
    input->key_draw = SDL_GetScancodeFromKey(SDLK_d);
    input->key_save = SDL_GetScancodeFromKey(SDLK_s);
    input->key_toggle_hitbox = SDL_GetScancodeFromKey(SDLK_c);
}

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
    input->switch_cursor = input->key[input->key_switch_cursor];
    input->tilemap_add = input->key[input->key_tilemap_add];
    input->tilemap_sub = input->key[input->key_tilemap_sub];
    input->erase = input->key[input->key_erase];
    input->draw = input->key[input->key_draw];
    input->save = input->key[input->key_save];
    input->toggle_hitbox = input->key[input->key_toggle_hitbox];
}

void resetKeyState_Input(SDL_Scancode key, Input *input)
{
    input->key[key] = SDL_FALSE;
}