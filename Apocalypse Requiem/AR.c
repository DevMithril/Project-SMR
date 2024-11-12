#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

/* resolution : 320x240 */
/* ligne de commande pour la compilation : gcc -o Apocalypse_Requiem AR.c $(sdl2-config --cflags --libs) */
/* pour utiliser valgrind (memoire) : valgrind -s --tool=memcheck --leak-check=yes|no|full|summary --leak-resolution=low|med|high --show-reachable=yes ./Apocalypse_Requiem */

typedef struct Hitbox
{
    SDL_bool opp_sides_parallels;
    int nb_points;
    SDL_Point *points;
    int cercle_x, cercle_y, cercle_rayon;
}Hitbox;

typedef struct Mob
{
    int Id;
    SDL_Texture *texture;
    int nb_sprites;
    SDL_Rect src_render_texture, target_render_texture;
    Hitbox hitbox;
    int x, y;
    struct Mob *suivant;
}Mob;

typedef struct Terrain_Hit
{
    int nb_hitboxes;
    Hitbox *hitboxes;
    int x, y;
}Terrain_Hit;

typedef struct Terrain_Aff
{
    int size_x, size_y;
    SDL_Texture *ground;
    SDL_Rect src_render_ground;
    SDL_Texture *background;
    SDL_Rect src_render_background;
    SDL_Texture *foreground;
    SDL_Rect src_render_foreground;
    int x, y;
}Terrain_Aff;

typedef struct Door
{
    Hitbox hitbox;
    int x, y;
}Door;

typedef struct Doors
{
    Door *doors;
    int nb_doors;
}Doors;

typedef struct Player
{
    SDL_Texture *texture;
    int nb_sprites;
    SDL_Rect src_render_texture, target_render_texture;
    Hitbox hitbox;
    int x, y;
}Player;

typedef struct All_Lists
{
    Terrain_Aff terrain_aff;
    Terrain_Hit terrain_hit;
    Doors doors;
    Mob *liste_mob;
    Player *player;
    SDL_Renderer *renderer;
    SDL_Window *window;
}All_Lists;

void destroy_Mob(Mob *mob, All_Lists *all_liste)
{
    Mob *liste = all_liste->liste_mob;
    if (mob == NULL)
    {
        printf("Erreur dans destroy_Mob : le mob passée en paramètre est un pointeur null \n");
        return;
    }
    while (mob != liste->suivant)
    {
        liste = liste->suivant;
    }

    Mob *next = liste->suivant->suivant;
    if (liste->suivant->texture != NULL)
    {
        SDL_DestroyTexture(liste->suivant->texture);
        liste->suivant->texture = NULL;
    }
    free(liste->suivant->hitbox.points);
    free(liste->suivant);
    liste->suivant = next;
}

void move_Mob(int x, int y, Mob *mob)
{
    mob->x += x;
    mob->y += y;
    mob->target_render_texture.x += x;
    mob->target_render_texture.y += y;
    mob->hitbox.cercle_x += x;
    mob->hitbox.cercle_y += y;
    for (int i = 0; i < mob->hitbox.nb_points-1 ; i++)
    {
        mob->hitbox.points[i].x += x;
        mob->hitbox.points[i].y += y;
    }
}

void affiche_Mob(Mob *mob, All_Lists *liste)
{
    if (mob->texture != NULL)
    {
        SDL_RenderCopy(liste->renderer, mob->texture, &mob->src_render_texture, &mob->target_render_texture);
    }
}

void destroy_Terrain(All_Lists *all_liste)
{
    if (all_liste->terrain_aff.ground != NULL)
    {
        SDL_DestroyTexture(all_liste->terrain_aff.ground);
        all_liste->terrain_aff.ground = NULL;
    }
    if (all_liste->terrain_aff.background != NULL)
    {
        SDL_DestroyTexture(all_liste->terrain_aff.background);
        all_liste->terrain_aff.background = NULL;
    }
    if (all_liste->terrain_aff.foreground != NULL)
    {
        SDL_DestroyTexture(all_liste->terrain_aff.foreground);
        all_liste->terrain_aff.foreground = NULL;
    }
    for (int i = 0; i < all_liste->terrain_hit.nb_hitboxes-1 ; i++)
    {
        free(all_liste->terrain_hit.hitboxes[i].points);
    }
    all_liste->terrain_hit.nb_hitboxes = 0;
    if (all_liste->terrain_hit.hitboxes != NULL)
    {
        free(all_liste->terrain_hit.hitboxes);
    }
    all_liste->terrain_hit.hitboxes = NULL;
}

void move_Terrain(int x, int y, All_Lists *liste)
{
    liste->terrain_aff.x += x;
    liste->terrain_hit.x += x;
    liste->terrain_aff.y += y;
    liste->terrain_hit.y += y;
    liste->terrain_aff.src_render_ground.x += x;
    liste->terrain_aff.src_render_ground.y += y;
    liste->terrain_aff.src_render_background.x = liste->terrain_aff.src_render_ground.x / 2;
    liste->terrain_aff.src_render_background.y = liste->terrain_aff.src_render_ground.y / 2;
    liste->terrain_aff.src_render_foreground.x = liste->terrain_aff.src_render_ground.x * 2;
    liste->terrain_aff.src_render_foreground.y = liste->terrain_aff.src_render_ground.y * 2;

    for (int i = 0; i < liste->terrain_hit.nb_hitboxes-1 ; i++)
    {
        liste->terrain_hit.hitboxes[i].cercle_x += x;
        liste->terrain_hit.hitboxes[i].cercle_y += y;
        for (int j = 0; j < liste->terrain_hit.hitboxes[i].nb_points-1 ; j++)
        {
            liste->terrain_hit.hitboxes[i].points[j].x += x;
            liste->terrain_hit.hitboxes[i].points[j].y += y;
        }
    }
}

void affiche_Terrain(All_Lists *liste)
{
    if (liste->terrain_aff.background != NULL)
    {
        SDL_RenderCopy(liste->renderer, liste->terrain_aff.background, &liste->terrain_aff.src_render_background, NULL);
    }
    if (liste->terrain_aff.ground != NULL)
    {
        SDL_RenderCopy(liste->renderer, liste->terrain_aff.ground, &liste->terrain_aff.src_render_ground, NULL);
    }
    if (liste->terrain_aff.foreground != NULL)
    {
        SDL_RenderCopy(liste->renderer, liste->terrain_aff.foreground, &liste->terrain_aff.src_render_foreground, NULL);
    }
}

void destroy_Doors(All_Lists *liste)
{
    for (int i = 0; i < liste->doors.nb_doors-1 ; i++)
    {
        free(liste->doors.doors[i].hitbox.points);
    }
    liste->doors.nb_doors = 0;
    if (liste->doors.doors != NULL)
    {
        free(liste->doors.doors);
    }
    liste->doors.doors = NULL;
}

void move_Doors(int x, int y, All_Lists *liste)
{
    for (int i = 0; i < liste->doors.nb_doors-1 ; i++)
    {
        for (int j = 0; j < liste->doors.doors[i].hitbox.nb_points ; j++)
        {
            liste->doors.doors[i].hitbox.points[j].x += x;
            liste->doors.doors[i].hitbox.points[j].y += y;
        }
        liste->doors.doors[i].hitbox.cercle_x += x;
        liste->doors.doors[i].hitbox.cercle_y += y;
        liste->doors.doors[i].x += x;
        liste->doors.doors[i].y += y;
    }
}

void destroy_Player(All_Lists *all_liste)
{
    if (all_liste->player == NULL)
    {
        printf("Erreur dans destroy_Player : le player est un pointeur null \n");
        return;
    }
    if (all_liste->player->texture != NULL)
    {
        SDL_DestroyTexture(all_liste->player->texture);
        all_liste->player->texture = NULL;
    }
    free(all_liste->player->hitbox.points);
    free(all_liste->player);
    all_liste->player = NULL;
}

void move_Player(int x, int y, Player *player)
{
    player->x += x;
    player->y += y;
    player->target_render_texture.x += x;
    player->target_render_texture.y += y;
    player->hitbox.cercle_x += x;
    player->hitbox.cercle_y += y;
    for (int i = 0; i < player->hitbox.nb_points-1 ; i++)
    {
        player->hitbox.points[i].x += x;
        player->hitbox.points[i].y += y;
    }
}

void affiche_Player(Player *player, All_Lists *liste)
{
    if (player->texture != NULL)
    {
        SDL_RenderCopy(liste->renderer, player->texture, &player->src_render_texture, &player->target_render_texture);
    }
}

void Quit(All_Lists *liste, int statut)
{
    /* liberation de la RAM allouee */

    while (NULL != liste->liste_mob->suivant)
    {
        destroy_Mob(liste->liste_mob->suivant, liste);
    }
    if (NULL == liste->liste_mob->suivant)
    {
        printf("Libération des Mobs effectuée\n");
    }
    destroy_Terrain(liste);
    if (liste->player != NULL)
    {
        destroy_Player(liste);
    }
    if (NULL == liste->player)
    {
        printf("Libération du Player effectuée\n");
    }

    /* destruction du renderer et de la fenetre, fermeture de la SDL puis sortie du programme */

    if (NULL != liste->renderer)
        SDL_DestroyRenderer(liste->renderer);
    if (NULL != liste->window)
        SDL_DestroyWindow(liste->window);
    SDL_Quit();
    printf("Libérations de toutes les ressources réussies\n");
    exit(statut);
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

Mob *load_Mob(const char chemin_file[], All_Lists *liste)
{
    FILE *fichier = NULL;
    Mob *new_mob = NULL;
    new_mob = malloc(sizeof(Mob));
    if (new_mob == NULL)
    {
        Quit(liste, EXIT_FAILURE);
    }
    char chemin_image[41];
    int opp_sides_parallels;

    fichier = fopen(chemin_file, "r");
    if (fichier != NULL)
    {
        fscanf(fichier, "%d %s %d %d %d ", &new_mob->Id, chemin_image, &new_mob->nb_sprites, &new_mob->target_render_texture.h, &new_mob->target_render_texture.w);
        fscanf(fichier, "%d %d %d %d ", &new_mob->target_render_texture.x, &new_mob->target_render_texture.y, &new_mob->hitbox.nb_points, &opp_sides_parallels);
        fscanf(fichier, "%d %d %d ", &new_mob->hitbox.cercle_x, &new_mob->hitbox.cercle_y, &new_mob->hitbox.cercle_rayon);
        if (opp_sides_parallels == 0)
        {
            new_mob->hitbox.opp_sides_parallels = SDL_FALSE;
        }
        else
        {
            new_mob->hitbox.opp_sides_parallels = SDL_TRUE;
        }
        SDL_Point points[new_mob->hitbox.nb_points];
        new_mob->hitbox.points = malloc(sizeof(points));
        if (new_mob->hitbox.points == NULL)
        {
            Quit(liste, EXIT_FAILURE);
        }
        for (int i = 0; i < new_mob->hitbox.nb_points; i++)
        {
            fscanf(fichier, " %d %d /", &new_mob->hitbox.points[i].x, &new_mob->hitbox.points[i].y);
        }
        fclose(fichier);
    }
    else
    {
        printf("L'ouverture du fichier %s a échouée\n", chemin_file);
    }
    if (strcmp(chemin_image,"none") != 0)
        new_mob->texture = loadImage(chemin_image, liste->renderer);
    else
        new_mob->texture == NULL;
    
    new_mob->x = 0;
    new_mob->y = 0;
    new_mob->src_render_texture.x = 0;
    new_mob->src_render_texture.y = 0;
    new_mob->src_render_texture.w = new_mob->target_render_texture.w;
    new_mob->src_render_texture.h = new_mob->target_render_texture.h;

    new_mob->suivant = NULL;
    while (NULL != liste->liste_mob->suivant)
        liste->liste_mob = liste->liste_mob->suivant;
    liste->liste_mob->suivant = new_mob;

    return new_mob;
}

void load_Terrain(const char chemin_file[], All_Lists *liste)
{
    FILE *fichier = NULL;
    destroy_Terrain(liste);
    char chemin_tilemap[41];
    SDL_Texture *tilemap = NULL;
    SDL_Rect src = {0, 0, 16, 16}, target = {0, 0, 16, 16};
    int opp_sides_parallels;

    fichier = fopen(chemin_file, "r");
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

void Scroll(int x, int y, All_Lists *liste)
{
    move_Terrain(x, y, liste);
    move_Doors(x, y, liste);
    Mob *mob_cible = liste->liste_mob->suivant;
    while (mob_cible != NULL)
    {
        move_Mob(x, y, mob_cible);
        mob_cible = mob_cible->suivant;
    }
}

SDL_bool sat(Hitbox *hitbox1, Hitbox *hitbox2)
{
    double min1 = 0, max1 = 0, min2 = 0, max2 = 0, p = 0;
    SDL_Point normal = {.x = 0, .y = 0};

    if ( (hitbox1->cercle_x-hitbox2->cercle_x)*(hitbox1->cercle_x-hitbox2->cercle_x) + (hitbox1->cercle_y-hitbox2->cercle_y)*(hitbox1->cercle_y-hitbox2->cercle_y) >
                     (hitbox1->cercle_rayon + hitbox2->cercle_rayon)*(hitbox1->cercle_rayon + hitbox2->cercle_rayon))
    {
        return SDL_FALSE;
    }

    for (int i = 0; i < (hitbox1->nb_points)-1; i++ )
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
    

    for (int i = 0; i < (hitbox2->nb_points)-1; i++ )
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

SDL_bool Collision_with_Terrain(Hitbox *hitbox, All_Lists *liste)
{
    for (int i = 0; i < liste->terrain_hit.nb_hitboxes; i++)
    {
        if (sat(hitbox, &liste->terrain_hit.hitboxes[i]))
        {
            return SDL_TRUE;
        }
    }
    return SDL_FALSE;
}

Mob *Collision_with_Mob(Hitbox *hitbox, All_Lists *liste)
{
    Mob *mob_cible = liste->liste_mob->suivant;
    while (mob_cible != NULL)
    {
        if (sat(hitbox, &mob_cible->hitbox))
        {
            return mob_cible;
        }
        mob_cible = mob_cible->suivant;
    }
    return NULL;
}

Door *Collision_with_Door(Hitbox *hitbox, All_Lists *liste)
{
    for (int i = 0; i < liste->doors.nb_doors-1 ; i++)
    {
        if (sat(hitbox, &liste->doors.doors[i].hitbox))
        {
            return &liste->doors.doors[i];
        }
    }
    return NULL;
}

typedef struct Input
{
    SDL_bool key[SDL_NUM_SCANCODES];
    SDL_bool quit;
}Input;

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

void Init(All_Lists *liste)
{
    /* initialisation de la SDL */

    if (0 != SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "Erreur SDL_Init : %s\n", SDL_GetError());
        Quit(liste, EXIT_FAILURE);
    }

    /* creation de la fenetre et du renderer associé */

    liste->window = SDL_CreateWindow("Apocalypse Requiem", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              320, 240, SDL_WINDOW_SHOWN /*| SDL_WINDOW_FULLSCREEN*/);
    if (NULL == liste->window)
    {
        fprintf(stderr, "Erreur SDL_CreateWindow : %s\n", SDL_GetError());
        Quit(liste, EXIT_FAILURE);
    }
    liste->renderer = SDL_CreateRenderer(liste->window, -1, SDL_RENDERER_ACCELERATED);
    if (NULL == liste->renderer)
    {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s\n", SDL_GetError());
        Quit(liste, EXIT_FAILURE);
    }
    SDL_RenderSetLogicalSize(liste->renderer, 320, 240);

    /* chargement et affichage de l'icone de la fenetre */

    SDL_Surface *icone;
    icone = SDL_LoadBMP("icone.bmp");
    if (NULL == icone)
    {
        fprintf(stderr, "Erreur SDL_LoadBMP : %s\n", SDL_GetError());
        Quit(liste, EXIT_FAILURE);
    }
    SDL_SetWindowIcon(liste->window, icone);
    SDL_FreeSurface(icone);
}

int main(int argc, char *argv[])
{
    /* Création des variables */

    Mob liste_mob = {.suivant = NULL};
    All_Lists all_lists = {.liste_mob = &liste_mob, .renderer = NULL, .window = NULL};
    Input input = {.quit = SDL_FALSE};
    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
        input.key[i] = SDL_FALSE;

    /* Initialisation, création de la fenêtre et du renderer. */

    Init(&all_lists);

    /* Boucle principale du jeu */

    while (!input.quit)
    {
        updateEvent(&input);

        
        SDL_RenderPresent(all_lists.renderer);
        SDL_Delay( (int)(1000 / 60) );
    }

    /* Fermeture du logiciel et libération de la mémoire */

    Quit(&all_lists, EXIT_SUCCESS);
}