#include "game_object.h"

Game_object *load_Game_object(FILE *file, SDL_Renderer *renderer)
{
    ;
}

void destroy_Game_object(Game_object *object)
{
    if (object->texture != NULL)
    {
        SDL_DestroyTexture(object->texture);
        object->texture = NULL;
    }
    destroy_Hitbox(&object->hitbox);
    free(object);
}

void move_Game_object(int x, int y, Game_object *object)
{
    move_Hitbox(x, y, &object->hitbox);
    object->dst_rect.x += x;
    object->dst_rect.y += y;
}

void display_Game_object(Game_object *object, SDL_Renderer *renderer)
{
    if (object->texture != NULL)
    {
        SDL_RenderCopy(renderer, object->texture, &object->src_rect, &object->dst_rect);
    }
}

void animate_Game_object(Game_object *object)
{
    if (object->src_rect.x + object->src_rect.w >= object->src_rect.w * object->nb_col)
    {
        object->src_rect.x = 0;
    }
    else
    {
        object->src_rect.x += object->src_rect.w;
    }
}

void chg_animation_Game_object(int animation_line, Game_object *object)
{
    if (animation_line + 1 <= object->nb_row)
    {
        object->src_rect.y = object->src_rect.h * animation_line;
    }
}

SDL_bool sat_Game_object(Game_object *object1, Game_object *object2)
{
    return sat(&object1->hitbox, &object2->hitbox);
}