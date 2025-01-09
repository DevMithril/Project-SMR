#include "level.h"

SDL_bool collision_Level(Hitbox *hitbox, Level *level)
{
    for (int i = 0; i < level->nb_hitboxes; i++)
    {
        if (sat(hitbox, &level->hitboxes[i]))
        {
            return SDL_TRUE;
        }
    }
    return SDL_FALSE;
}

void load_Level(FILE **file, Level *level, SDL_Renderer *renderer)
{
    level->texture = loadImage("assets/texture.bmp", renderer);
    level->src.x = 0;
    level->src.y = 0;
    level->src.h = 240;
    level->src.w = 320;
    level->nb_hitboxes = 0;
    level->hitboxes = NULL;
}

void destroy_Level(Level *level)
{
    if (level->texture != NULL)
    {
        SDL_DestroyTexture(level->texture);
    }
    if (level->hitboxes != NULL)
    {
        free(level->hitboxes);
    }
}

void move_cam_Level(int x, int y, Level *level)
{
    level->src.x += x;
    level->src.y += y;
}

void display_Level(Level *level, SDL_Renderer *renderer)
{
    if (level->texture != NULL)
    {
        SDL_RenderCopy(renderer, level->texture, &level->src, NULL);
    }
}