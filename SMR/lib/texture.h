#ifndef TEXTURE__LIB__H
#define TEXTURE__LIB__H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

SDL_Texture *loadImage(const char chemin[], SDL_Renderer *renderer);

#endif