#include "hitbox.h"

void get_normal(Hitbox *hitbox, int i, SDL_Point *normal)
{
    if (i+1 >= NB_POINTS)
    {
        normal->x = -(hitbox->points[i].y - hitbox->points[0].y);
        normal->y = hitbox->points[i].x - hitbox->points[0].x;
    }
    else
    {
        normal->x = -(hitbox->points[i].y - hitbox->points[i+1].y);
        normal->y = hitbox->points[i].x - hitbox->points[i+1].x;
    }
}

void get_min_max(double *min, double *max, Hitbox *hitbox, SDL_Point *normal)
{
    double p = 0;
    *min = normal->x * hitbox->points[0].x + normal->y * hitbox->points[0].y;
    *max = *min;
    for (int j = 1; j < NB_POINTS; j++ )
    {
        p = normal->x * hitbox->points[j].x + normal->y * hitbox->points[j].y;
        if (p < *min)
        {
            *min = p;
        }
        else if (p > *max)
        {
            *max = p;
        }
    }
}

SDL_bool sat(Hitbox *hitbox1, Hitbox *hitbox2)
{
    double min1 = 0, max1 = 0, min2 = 0, max2 = 0;
    SDL_Point normal = {.x = 0, .y = 0};

    for (int i = 0; i < NB_POINTS; i++ )
    {
        get_normal(hitbox1, i, &normal);
        get_min_max(&min1, &max1, hitbox1, &normal);
        get_min_max(&min2, &max2, hitbox2, &normal);
        if (min1 >= max2 || min2 >= max1)
        {
            return SDL_FALSE;
        }
    }

    for (int i = 0; i < NB_POINTS; i++ )
    {
        get_normal(hitbox2, i, &normal);
        get_min_max(&min1, &max1, hitbox1, &normal);
        get_min_max(&min2, &max2, hitbox2, &normal);
        if (min1 >= max2 || min2 >= max1)
        {
            return SDL_FALSE;
        }
    }

    return SDL_TRUE;
}

void move_Hitbox(int x, int y, Hitbox *hitbox)
{
    for (int i = 0; i < NB_POINTS; i++)
    {
        hitbox->points[i].x += x;
        hitbox->points[i].y += y;
    }
}

void display_Hitbox(Hitbox *hitbox, SDL_Renderer *renderer)
{
    int i;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (i = 0; i < NB_POINTS; i++)
    {
        SDL_RenderDrawLine(renderer, hitbox->points[i].x, hitbox->points[i].y,
                                    hitbox->points[i+1].x, hitbox->points[i+1].y);
    }
    SDL_RenderDrawLine(renderer, hitbox->points[i].x, hitbox->points[i].y,
                                hitbox->points[0].x, hitbox->points[0].y);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}