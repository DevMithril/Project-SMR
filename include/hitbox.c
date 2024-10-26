#include "hitbox.h"

void get_normal(Hitbox *hitbox, int i, SDL_Point *normal)
{
    if (i+1 >= hitbox->nb_points)
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
    for (int j = 1; j < hitbox->nb_points; j++ )
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

int square(int x)
{
    return x*x;
}

SDL_bool sat(Hitbox *hitbox1, Hitbox *hitbox2)
{
    double min1 = 0, max1 = 0, min2 = 0, max2 = 0;
    SDL_Point normal = {.x = 0, .y = 0};

    if (square(hitbox1->cercle_x-hitbox2->cercle_x) + square(hitbox1->cercle_y-hitbox2->cercle_y) > square(hitbox1->cercle_rayon + hitbox2->cercle_rayon))
    {
        return SDL_FALSE;
    }

    for (int i = 0; i < hitbox1->nb_points; i++ )
    {
        get_normal(hitbox1, i, &normal);
        get_min_max(&min1, &max1, hitbox1, &normal);
        get_min_max(&min2, &max2, hitbox2, &normal);
        if (min1 >= max2 || min2 >= max1)
        {
            return SDL_FALSE;
        }
    }

    for (int i = 0; i < hitbox2->nb_points; i++ )
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

void load_Hitbox(FILE *file, Hitbox *hitbox)
{
    ;
}

void destroy_Hitbox(Hitbox *hitbox)
{
    hitbox->cercle_rayon = 0;
    hitbox->cercle_x = 0;
    hitbox->cercle_y = 0;
    hitbox->nb_points = 0;
    if (hitbox->points != NULL)
    {
        free(hitbox->points);
        hitbox->points = NULL;
    }
}

void move_Hitbox(int x, int y, Hitbox *hitbox)
{
    hitbox->cercle_x += x;
    hitbox->cercle_y += y;
    for (int i = 0; i < hitbox->nb_points; i++)
    {
        hitbox->points[i].x += x;
        hitbox->points[i].y += y;
    }
}