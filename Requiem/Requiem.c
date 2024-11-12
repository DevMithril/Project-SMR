#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

/* resolution : 320x240 */
/* ligne de commande pour la compilation : gcc -o Requiem Requiem.c $(sdl2-config --cflags --libs) */
/* pour utiliser valgrind (memoire) : valgrind -s --tool=memcheck --leak-check=yes|no|full|summary --leak-resolution=low|med|high --show-reachable=yes ./Requiem */

typedef struct Hitbox
{
    SDL_bool opp_sides_parallels;
    int nb_points;
    SDL_Point *points;
    int cercle_x, cercle_y, cercle_rayon;
}Hitbox;

typedef struct Level
{
    int size_x, size_y, x, y;
    SDL_Texture *ground, *background;
    SDL_Rect src_render_ground;
    int nb_hitboxes;
    Hitbox *hitboxes;
}Level;

typedef struct Input
{
    SDL_bool key[SDL_NUM_SCANCODES];
    SDL_bool quit;
}Input;

typedef struct Everything
{
    Level level;
    Input input;
    SDL_Renderer *renderer;
    SDL_Window *window;
}Everything;

void updateEvent(Input *input)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
            input->quit = SDL_TRUE;
        else if(event.type == SDL_KEYDOWN)
            input->key[event.key.keysym.scancode] = SDL_TRUE;
        else if(event.type == SDL_KEYUP)
            input->key[event.key.keysym.scancode] = SDL_FALSE;
    }
}

SDL_Texture *loadImage(const char chemin[], SDL_Renderer *renderer)
{
    SDL_Surface *surface = NULL; 
    SDL_Texture *texture = NULL, *tmp = NULL;
    surface = SDL_LoadBMP(chemin);
    if(NULL == surface)
    {
        fprintf(stderr, "Erreur SDL_LoadBMP : %s\n", SDL_GetError());
        return NULL;
    }
    tmp = SDL_CreateTextureFromSurface(renderer, surface);
    if(NULL == tmp)
    {
        fprintf(stderr, "Erreur SDL_CreateTextureFromSurface : %s\n", SDL_GetError());
        return NULL;
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, 
                            SDL_TEXTUREACCESS_TARGET, surface->w, surface->h);
    if(NULL == texture)
    {
        fprintf(stderr, "Erreur SDL_CreateTexture : %s\n", SDL_GetError());
        return NULL;
    }
    SDL_SetRenderTarget(renderer, texture);
    SDL_RenderCopy(renderer, tmp, NULL, NULL);
    SDL_DestroyTexture(tmp);
    SDL_FreeSurface(surface);
    SDL_SetRenderTarget(renderer, NULL);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return texture;
}

SDL_bool sat(Hitbox *hitbox1, Hitbox *hitbox2)
{
    double min1 = 0, max1 = 0, min2 = 0, max2 = 0, p = 0;
    int nb_axes1, nb_axes2;
    SDL_Point normal = {.x = 0, .y = 0};

    if ( (hitbox1->cercle_x-hitbox2->cercle_x)*(hitbox1->cercle_x-hitbox2->cercle_x) + (hitbox1->cercle_y-hitbox2->cercle_y)*(hitbox1->cercle_y-hitbox2->cercle_y) >
                     (hitbox1->cercle_rayon + hitbox2->cercle_rayon)*(hitbox1->cercle_rayon + hitbox2->cercle_rayon))
    {
        return SDL_FALSE;
    }

    if (hitbox1->opp_sides_parallels)
    {
        nb_axes1 = hitbox1->nb_points / 2;
    }
    else
    {
        nb_axes1 = hitbox1->nb_points;
    }

    if (hitbox2->opp_sides_parallels)
    {
        nb_axes2 = hitbox2->nb_points / 2;
    }
    else
    {
        nb_axes2 = hitbox2->nb_points;
    }

    for (int i = 0; i < nb_axes1-1; i++ )
    {
        if (i+1 > hitbox1->nb_points-1)
        {
            normal.x = -(hitbox1->points[i].y - hitbox1->points[0].y);
            normal.y = hitbox1->points[i].x - hitbox1->points[0].x;
        }
        else
        {
            normal.x = -(hitbox1->points[i].y - hitbox1->points[i+1].y);
            normal.y = hitbox1->points[i].x - hitbox1->points[i+1].x;
        }
        min1 = normal.x * hitbox1->points[0].x + normal.y * hitbox1->points[0].y;
        max1 = min1;
        for (int j = 1; j < (hitbox1->nb_points)-1; j++ )
        {
            p = normal.x * hitbox1->points[j].x + normal.y * hitbox1->points[j].y;
            if (p < min1)
            {
                min1 = p;
            }
            else if (p > max1)
            {
                max1 = p;
            }
        }
        min2 = normal.x * hitbox2->points[0].x + normal.y * hitbox2->points[0].y;
        max2 = min2;
        for (int j = 1; j < (hitbox2->nb_points)-1; j++ )
        {
            p = normal.x * hitbox2->points[j].x + normal.y * hitbox2->points[j].y;
            if (p < min2)
            {
                min2 = p;
            }
            else if (p > max2)
            {
                max2 = p;
            }
        }
        if (min1 > max2 || min2 > max1)
        {
            return SDL_FALSE;
        }
    }
    

    for (int i = 0; i < nb_axes2-1; i++ )
    {
        if (i+1 > hitbox2->nb_points-1)
        {
            normal.x = -(hitbox2->points[i].y - hitbox2->points[0].y);
            normal.y = hitbox2->points[i].x - hitbox2->points[0].x;
        }
        else
        {
            normal.x = -(hitbox2->points[i].y - hitbox2->points[i+1].y);
            normal.y = hitbox2->points[i].x - hitbox2->points[i+1].x;
        }
        min1 = normal.x * hitbox1->points[0].x + normal.y * hitbox1->points[0].y;
        max1 = min1;
        for (int j = 1; j < (hitbox1->nb_points)-1; j++ )
        {
            p = normal.x * hitbox1->points[j].x + normal.y * hitbox1->points[j].y;
            if (p < min1)
            {
                min1 = p;
            }
            else if (p > max1)
            {
                max1 = p;
            }
        }
        min2 = normal.x * hitbox2->points[0].x + normal.y * hitbox2->points[0].y;
        max2 = min2;
        for (int j = 1; j < (hitbox2->nb_points)-1; j++ )
        {
            p = normal.x * hitbox2->points[j].x + normal.y * hitbox2->points[j].y;
            if (p < min2)
            {
                min2 = p;
            }
            else if (p > max2)
            {
                max2 = p;
            }
        }
        if (min1 > max2 || min2 > max1)
        {
            return SDL_FALSE;
        }
    }

    return SDL_TRUE;
}

void destroy_Level(Everything *all)
{
    if (all->level.ground != NULL)
    {
        SDL_DestroyTexture(all->level.ground);
    }
    if (all->level.background)
    {
        SDL_DestroyTexture(all->level.background);
    }
    if (all->level.hitboxes != NULL)
    {
        for (int i = 0; i < all->level.nb_hitboxes-1; i++)
        {
            free(all->level.hitboxes[i].points);
        }
        free(all->level.hitboxes);
    }
    all->level.nb_hitboxes = 0;
}

void Quit(Everything *all, int statut)
{
    /* liberation de la RAM allouee */

    destroy_Level(all);

    /* destruction du renderer et de la fenetre, fermeture de la SDL puis sortie du programme */

    if (NULL != all->renderer)
        SDL_DestroyRenderer(all->renderer);
    if (NULL != all->window)
        SDL_DestroyWindow(all->window);
    SDL_Quit();
    printf("Libérations de toutes les ressources réussies\n");
    exit(statut);
}

void load_Level(const char chemin[], Everything *all)
{
    FILE *fichier = NULL;
    destroy_Level(all);
    char chemin_tilemap[41];
    SDL_Texture *tilemap = NULL;
    SDL_Rect src = {0, 0, 16, 16}, target = {0, 0, 16, 16};
    int opp_sides_parallels;

    fichier = fopen(chemin, "r");
    if (fichier != NULL)
    {
        /* lecture des textures du terrain */

        fscanf(fichier, "%s %d %d //", chemin_tilemap, &liste->terrain_aff.size_x, &liste->terrain_aff.size_y);
        tilemap = loadImage(chemin_tilemap, liste->renderer);

        liste->terrain_aff.ground = SDL_CreateTexture(liste->renderer, SDL_PIXELFORMAT_RGBA8888, 
                                                        SDL_TEXTUREACCESS_TARGET, liste->terrain_aff.size_x * 16, liste->terrain_aff.size_y * 16);
        if(NULL == liste->terrain_aff.ground)
        {
            fprintf(stderr, "Erreur SDL_CreateTexture : %s\n", SDL_GetError());
            return;
        }
        SDL_SetRenderTarget(liste->renderer, liste->terrain_aff.ground);
        for (int v = 0; v < liste->terrain_aff.size_y; v++)
        {
            for (int w = 0; w < liste->terrain_aff.size_x; w++)
            {
                target.x = w * 16;
                target.y = v * 16;
                fscanf(fichier, " %d %d /", &src.x, &src.y);
                src.x = src.x * 16;
                src.y = src.y * 16;
                SDL_RenderCopy(liste->renderer, tilemap, &src, &target);
            }
        }
        SDL_SetRenderTarget(liste->renderer, NULL);

        liste->terrain_aff.background = SDL_CreateTexture(liste->renderer, SDL_PIXELFORMAT_RGBA8888, 
                                                        SDL_TEXTUREACCESS_TARGET, liste->terrain_aff.size_x * 8, liste->terrain_aff.size_y * 8);
        if(NULL == liste->terrain_aff.background)
        {
            fprintf(stderr, "Erreur SDL_CreateTexture : %s\n", SDL_GetError());
            return;
        }
        SDL_SetRenderTarget(liste->renderer, liste->terrain_aff.background);
        for (int v = 0; v < liste->terrain_aff.size_y / 2; v++)
        {
            for (int w = 0; w < liste->terrain_aff.size_x / 2; w++)
            {
                target.x = w * 16;
                target.y = v * 16;
                fscanf(fichier, " %d %d /", &src.x, &src.y);
                src.x = src.x * 16;
                src.y = src.y * 16;
                SDL_RenderCopy(liste->renderer, tilemap, &src, &target);
            }
        }
        SDL_SetRenderTarget(liste->renderer, NULL);

        liste->terrain_aff.foreground = SDL_CreateTexture(liste->renderer, SDL_PIXELFORMAT_RGBA8888, 
                                                        SDL_TEXTUREACCESS_TARGET, liste->terrain_aff.size_x * 32, liste->terrain_aff.size_y * 32);
        if(NULL == liste->terrain_aff.foreground)
        {
            fprintf(stderr, "Erreur SDL_CreateTexture : %s\n", SDL_GetError());
            return;
        }
        SDL_SetRenderTarget(liste->renderer, liste->terrain_aff.foreground);
        for (int v = 0; v < liste->terrain_aff.size_y * 2; v++)
        {
            for (int w = 0; w < liste->terrain_aff.size_x * 2; w++)
            {
                target.x = w * 16;
                target.y = v * 16;
                fscanf(fichier, " %d %d /", &src.x, &src.y);
                src.x = src.x * 16;
                src.y = src.y * 16;
                SDL_RenderCopy(liste->renderer, tilemap, &src, &target);
            }
        }
        SDL_SetRenderTarget(liste->renderer, NULL);

        liste->terrain_aff.x = 0;
        liste->terrain_aff.y = 0;
        liste->terrain_hit.x = 0;
        liste->terrain_hit.y = 0;
        liste->terrain_aff.src_render_ground.x = 0;
        liste->terrain_aff.src_render_ground.y = 0;
        liste->terrain_aff.src_render_background.x = 0;
        liste->terrain_aff.src_render_background.y = 0;
        liste->terrain_aff.src_render_foreground.x = 0;
        liste->terrain_aff.src_render_foreground.y = 0;
        liste->terrain_aff.src_render_ground.w = 320;
        liste->terrain_aff.src_render_ground.h = 240;
        liste->terrain_aff.src_render_background.w = 320;
        liste->terrain_aff.src_render_background.h = 240;
        liste->terrain_aff.src_render_foreground.w = 320;
        liste->terrain_aff.src_render_foreground.h = 240;

        /* lecture des hitboxes du terrain */

        fscanf(fichier, "/ %d //", &liste->terrain_hit.nb_hitboxes);
        Hitbox hitbox[liste->terrain_hit.nb_hitboxes];
        liste->terrain_hit.hitboxes = malloc(sizeof(hitbox));
        if (liste->terrain_hit.hitboxes == NULL)
        {
            Quit(liste, EXIT_FAILURE);
        }
        for (int i = 0; i < liste->terrain_hit.nb_hitboxes; i++)
        {
            fscanf(fichier, " %d %d ", &liste->terrain_hit.hitboxes[i].nb_points, &opp_sides_parallels);
            fscanf(fichier, "%d %d %d //", &liste->terrain_hit.hitboxes[i].cercle_x, &liste->terrain_hit.hitboxes[i].cercle_y, &liste->terrain_hit.hitboxes[i].cercle_rayon);
            if (opp_sides_parallels == 0)
            {
                liste->terrain_hit.hitboxes[i].opp_sides_parallels = SDL_FALSE;
            }
            else
            {
                liste->terrain_hit.hitboxes[i].opp_sides_parallels = SDL_TRUE;
            }

            SDL_Point points[liste->terrain_hit.hitboxes[i].nb_points];
            liste->terrain_hit.hitboxes[i].points = malloc(sizeof(points));
            if (liste->terrain_hit.hitboxes[i].points == NULL)
            {
                Quit(liste, EXIT_FAILURE);
            }
            for (int j = 0; j < liste->terrain_hit.hitboxes[i].nb_points; j++)
            {
                fscanf(fichier, " %d %d /", &liste->terrain_hit.hitboxes[i].points[j].x, &liste->terrain_hit.hitboxes[i].points[j].y);
            }
        }
        fclose(fichier);
    }
    else
    {
        printf("L'ouverture du fichier %s a échouée\n", chemin_file);
    }
}

void Init(Everything *all)
{
    /* initialisation de la SDL */

    if (0 != SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "Erreur SDL_Init : %s\n", SDL_GetError());
        Quit(all, EXIT_FAILURE);
    }

    /* creation de la fenetre et du renderer associé */

    all->window = SDL_CreateWindow("Requiem", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              4*320, 4*240, SDL_WINDOW_SHOWN /*| SDL_WINDOW_FULLSCREEN*/);
    if (NULL == all->window)
    {
        fprintf(stderr, "Erreur SDL_CreateWindow : %s\n", SDL_GetError());
        Quit(all, EXIT_FAILURE);
    }
    all->renderer = SDL_CreateRenderer(all->window, -1, SDL_RENDERER_ACCELERATED);
    if (NULL == all->renderer)
    {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s\n", SDL_GetError());
        Quit(all, EXIT_FAILURE);
    }
    SDL_RenderSetLogicalSize(all->renderer, 320, 240);

    /* chargement et affichage de l'icone de la fenetre */

    SDL_Surface *icone;
    icone = SDL_LoadBMP("icone.bmp");
    if (NULL == icone)
    {
        fprintf(stderr, "Erreur SDL_LoadBMP : %s\n", SDL_GetError());
        Quit(all, EXIT_FAILURE);
    }
    SDL_SetWindowIcon(all->window, icone);
    SDL_FreeSurface(icone);
}

int main(int argc, char *argv[])
{
    /* Création des variables */

    Everything all = {.renderer = NULL, .window = NULL};
    all.input.quit = SDL_FALSE;
    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
        all.input.key[i] = SDL_FALSE;

    /* Initialisation, création de la fenêtre et du renderer. */

    Init(&all);

    /* Boucle principale du jeu */

    while (!all.input.quit)
    {
        updateEvent(&all.input);
        SDL_RenderClear(all.renderer);

        
        SDL_RenderPresent(all.renderer);
        SDL_Delay( (int)(1000 / 60) );
    }

    /* Fermeture du logiciel et libération de la mémoire */

    Quit(&all, EXIT_SUCCESS);
}