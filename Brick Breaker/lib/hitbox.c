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
    *min = normal->x * (hitbox->points[0].x + hitbox->cercle_x) + normal->y * (hitbox->points[0].y + hitbox->cercle_y);
    *max = *min;
    for (int j = 1; j < hitbox->nb_points; j++ )
    {
        p = normal->x * (hitbox->points[j].x + hitbox->cercle_x) + normal->y * (hitbox->points[j].y + hitbox->cercle_y);
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

void save_Hitbox(FILE **file, Hitbox *hitbox)
{
    fwrite(hitbox, sizeof(Hitbox), 1, *file);
    fwrite(hitbox->points, sizeof(SDL_Point), hitbox->nb_points, *file);
}

Hitbox *load_Hitbox(FILE **file)
{
    Hitbox *hitbox = NULL;
    hitbox = malloc(sizeof(Hitbox));
    if (NULL == hitbox)
    {
        fprintf(stderr, "Erreur dans load_Hitbox : Out of Memory\n");
        return NULL;
    }
    fread(hitbox, sizeof(Hitbox), 1, *file);
    hitbox->points = NULL;
    hitbox->points = malloc(hitbox->nb_points * sizeof(SDL_Point));
    if (NULL == hitbox->points)
    {
        fprintf(stderr, "Erreur dans load_Hitbox : Out of Memory\n");
        free(hitbox);
        return NULL;
    }
    fread(hitbox->points, sizeof(SDL_Point), hitbox->nb_points, *file);
    return hitbox;
}

void destroy_Hitbox(Hitbox **hitbox)
{
    if (*hitbox != NULL)
    {
        if ((*hitbox)->points != NULL)
        {
            free((*hitbox)->points);
        }
        free(*hitbox);
        *hitbox = NULL;
    }
}

void move_Hitbox(int x, int y, Hitbox *hitbox)
{
    hitbox->cercle_x += x;
    hitbox->cercle_y += y;
}

void display_Hitbox(Hitbox *hitbox, SDL_Renderer *renderer)
{
    int i = 0;
    int offsetx = hitbox->cercle_x, offsety = hitbox->cercle_y;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (i = 0; i < hitbox->nb_points - 1; i++)
    {
        SDL_RenderDrawLine(renderer, offsetx + hitbox->points[i].x, offsety + hitbox->points[i].y,
                                    offsetx + hitbox->points[i+1].x, offsety + hitbox->points[i+1].y);
    }
    SDL_RenderDrawLine(renderer, offsetx + hitbox->points[i].x, offsety + hitbox->points[i].y,
                                offsetx + hitbox->points[0].x, offsety + hitbox->points[0].y);
    SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);
    SDL_RenderDrawLine(renderer, offsetx + hitbox->points[0].x - 3, offsety + hitbox->points[0].y,
                                offsetx + hitbox->points[0].x + 3, offsety + hitbox->points[0].y);
    SDL_RenderDrawLine(renderer, offsetx + hitbox->points[0].x, offsety + hitbox->points[0].y - 3,
                                offsetx + hitbox->points[0].x, offsety + hitbox->points[0].y + 3);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}