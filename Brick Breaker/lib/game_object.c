#include "game_object.h"

void save_Game_object(FILE **file, Game_object *object, const char *texture_file)
{
    char texture[GAME_OBJECT_LEN_TEXTURE_FILE];
    strcpy(texture, texture_file);
    fwrite(texture, sizeof(texture), 1, *file);
    fwrite(object, sizeof(Game_object), 1, *file);
    for (int i = 0; i < object->nb_row; i++)
    {
        save_Hitbox(file, object->hitboxes[i]);
    }
}

Game_object *load_Game_object(FILE **file, SDL_Renderer *renderer)
{
    Game_object *object = NULL;
    char texture_file[GAME_OBJECT_LEN_TEXTURE_FILE];
    object = malloc(sizeof(Game_object));
    if (NULL == object)
    {
        fprintf(stderr, "Erreur dans load_Game_object : Out of Memory\n");
        return NULL;
    }
    fread(texture_file, sizeof(texture_file), 1, *file);
    object->texture = loadImage(texture_file, renderer);
    object->hitboxes = malloc(object->nb_row * sizeof(Hitbox*));
    if (NULL == object->hitboxes)
    {
        fprintf(stderr, "Erreur dans load_Game_object : Out of Memory\n");
        free(object);
        return NULL;
    }
    for (int i = 0; i < object->nb_row; i++)
    {
        object->hitboxes[i] = load_Hitbox(file);
        if (NULL == object->hitboxes[i])
        {
            for (int j = 0; j < i; j++)
            {
                destroy_Hitbox(&object->hitboxes[j]);
            }
            free(object->hitboxes);
            free(object);
            return NULL;
        }
    }
    object->current_hitbox = object->hitboxes[0];
    return object;
}

void destroy_Game_object(Game_object **object)
{
    if ((*object) == NULL)
    {
        return;
    }
    if ((*object)->texture != NULL)
    {
        SDL_DestroyTexture((*object)->texture);
        (*object)->texture = NULL;
    }
    if ((*object)->hitboxes != NULL)
    {
        for (int i = 0; i < (*object)->nb_row; i++)
        {
            destroy_Hitbox(&(*object)->hitboxes[i]);
        }
        free((*object)->hitboxes);
    }
    free((*object));
    *object = NULL;
}

void move_Game_object(int x, int y, Game_object *object)
{
    for (int i = 0; i < object->nb_row; i++)
    {
        move_Hitbox(x, y, object->hitboxes[i]);
    }
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
    if (animation_line + 1 <= object->nb_row && animation_line >= 0)
    {
        object->src_rect.y = object->src_rect.h * animation_line;
        object->src_rect.x = 0;
        object->current_hitbox = object->hitboxes[animation_line];
    }
}

SDL_bool sat_Game_object(Game_object *object1, Game_object *object2)
{
    return sat(object1->current_hitbox, object2->current_hitbox);
}