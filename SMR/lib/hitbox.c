#include "hitbox.h"

void get_middle(Hitbox *hitbox, Vector2 *middle)
{
    for (int i = 0; i < NB_POINTS; i++)
    {
        middle->x += hitbox->points[i].x;
        middle->y += hitbox->points[i].y;
    }
    middle->x /= NB_POINTS;
    middle->y /= NB_POINTS;
}

void get_normal(Hitbox *hitbox, int i, Vector2 *normal)
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
    double magnitude = sqrt(normal->x * normal->x + normal->y * normal->y);
    normal->x = normal->x / magnitude;
    normal->y = normal->y / magnitude;
}

void get_min_max(double *min, double *max, Hitbox *hitbox, Vector2 *normal)
{
    double p = 0;
    *min = normal->x * hitbox->points[0].x + normal->y * hitbox->points[0].y;
    *max = *min;
    for (int j = 1; j < NB_POINTS; j++)
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

void get_smallest_overlap(double *overlap, Vector2 *smallest, double *max1, double *max2, double *min1, double *min2, Vector2 *normal)
{
    double o;
    if (*max1 - *min2 <= *max2 - *min1)
    {
        o = *max1 - *min2;
    }
    else
    {
        o = *max2 - *min1;
    }
    if (o < *overlap || *overlap == -1)
    {
        *overlap = o;
        *smallest = *normal;
    }
}

Vector2 sat(Hitbox *hitbox1, Hitbox *hitbox2)
{
    double min1, max1, min2, max2, overlap = -1;
    Vector2 null = {.x = 0, .y = 0};
    Vector2 middle1 = null, middle2 = null, normal, smallest;

    for (int i = 0; i < NB_POINTS; i++)
    {
        get_normal(hitbox1, i, &normal);
        get_min_max(&min1, &max1, hitbox1, &normal);
        get_min_max(&min2, &max2, hitbox2, &normal);
        if (min1 >= max2 || min2 >= max1)
        {
            return null;
        }
        get_smallest_overlap(&overlap, &smallest, &max1, &max2, &min1, &min2, &normal);
    }

    for (int i = 0; i < NB_POINTS; i++)
    {
        get_normal(hitbox2, i, &normal);
        get_min_max(&min1, &max1, hitbox1, &normal);
        get_min_max(&min2, &max2, hitbox2, &normal);
        if (min1 >= max2 || min2 >= max1)
        {
            return null;
        }
        get_smallest_overlap(&overlap, &smallest, &max1, &max2, &min1, &min2, &normal);
    }

    get_middle(hitbox1, &middle1);
    get_middle(hitbox2, &middle2);
    if ((middle2.x - middle1.x) * smallest.x + (middle2.y - middle1.y) * smallest.y > 0)
    {
        smallest.x = -smallest.x;
        smallest.y = -smallest.y;
    }
    smallest.x = smallest.x * overlap;
    smallest.y = smallest.y * overlap;
    return smallest;
}

void move_Hitbox(int x, int y, Hitbox *hitbox)
{
    for (int i = 0; i < NB_POINTS; i++)
    {
        hitbox->points[i].x += x;
        hitbox->points[i].y += y;
    }
}

void display_Hitbox(Hitbox *hitbox, int offset_x, int offset_y, SDL_Renderer *renderer)
{
    int i;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (i = 0; i < NB_POINTS-1; i++)
    {
        SDL_RenderDrawLine(renderer, hitbox->points[i].x + offset_x, hitbox->points[i].y + offset_y,
                                    hitbox->points[i+1].x + offset_x, hitbox->points[i+1].y + offset_y);
    }
    SDL_RenderDrawLine(renderer, hitbox->points[i].x + offset_x, hitbox->points[i].y + offset_y,
                                hitbox->points[0].x + offset_x, hitbox->points[0].y + offset_y);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}