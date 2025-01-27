#ifndef TEXTURE__LIB__H
#define TEXTURE__LIB__H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

SDL_Texture *loadImage(const char chemin[], SDL_Renderer *renderer);

void render_Texture(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect *src, SDL_Rect *dst);

#endif