#ifndef GAME__LIB__H
#define GAME__LIB__H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "everything.h"
#include "level.h"

/* initialise le jeu */
void initGame(Everything *all);

/* execute une frame du jeu */
void runGame(Everything *all);

#endif