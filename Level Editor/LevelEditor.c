#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* resolution : 640x480 (2x la resolution du jeu) */
/* ligne de commande pour la compilation : gcc -o LevelEditor LevelEditor.c -lm $(sdl2-config --cflags --libs) -l SDL_ttf */
/* pour utiliser valgrind (memoire) : valgrind -s --tool=memcheck --leak-check=yes|no|full|summary --leak-resolution=low|med|high --show-reachable=yes ./LevelEditor */

typedef struct Hitbox
{
    SDL_bool opp_sides_parallels;
    int nb_points;
    SDL_Point *points;
    int cercle_x, cercle_y, cercle_rayon;
    struct Hitbox *suivant;
}Hitbox;

typedef struct Point
{
    SDL_Point point;
    struct Point *suivant;
}Point;

typedef struct Tile
{
    char *tilemap;
    int x, y;
}Tile;

typedef struct Lign_of_Tiles
{
    Tile *ligne;
}Lign_of_Tiles;

typedef struct Cursor
{
    int Id;
    SDL_Texture *texture;
    SDL_Rect target_render_texture;
    int x, y;
}Cursor;

typedef struct Tilemap
{
    char *chemin_tilemap;
    SDL_Texture *texture;
    struct Tilemap *suivant;
    struct Tilemap *precedent;
}Tilemap;

typedef struct Item
{
    SDL_Texture *texture;
    SDL_Rect src_render_texture, target_render_texture;
    int x, y;
    struct Item *suivant;
}Item;

typedef struct Mob
{
    SDL_Texture *texture;
    SDL_Rect src_render_texture, target_render_texture;
    int x, y;
    struct Mob *suivant;
}Mob;

typedef struct Level
{
    char *chemin_level;
    int size_x, size_y;
    Lign_of_Tiles *current_matrice, *matrice_ground, *matrice_background;
    SDL_Texture *current_target, *ground, *background, *background1, *background2, *background3, *background4;
    SDL_Rect src_render_ground, src_render_background1, src_render_background2, src_render_background3, src_render_background4;
    int x, y;
}Level;

typedef struct All_Lists
{
    Cursor cursor_tilemap, cursor_level, cursor_hitbox;
    Tilemap *liste_tilemap;
    Tilemap *current_tilemap;
    Level level;
    Item *liste_item;
    Mob *liste_mob;
    Hitbox *liste_hitbox;
    Point *liste_point;
    int mode;
    SDL_Texture *help_screen, *new_level_screen, *numbers, *void_background, *mode_tilemap_help, *mode_hitbox_help, *hitbox_mode, *level_mode_target, *save_mode_help;
    SDL_Renderer *renderer;
    SDL_Window *window;
}All_Lists;

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

void destroy_Hitbox(Hitbox *hitbox, All_Lists *all_liste)
{
    Hitbox *liste = all_liste->liste_hitbox;
    if (hitbox == NULL)
    {
        printf("Erreur dans destroy_Hitbox : la hitbox passée en paramètre est un pointeur null \n");
        return;
    }
    while (hitbox != liste->suivant)
    {
        liste = liste->suivant;
    }

    Hitbox *next = liste->suivant->suivant;
    free(liste->suivant->points);
    free(liste->suivant);
    liste->suivant = next;
}

void destroy_Point(Point *point, All_Lists *all_liste)
{
    Point *liste = all_liste->liste_point;
    if (point == NULL)
    {
        printf("Erreur dans destroy_Point : le point passée en paramètre est un pointeur null \n");
        return;
    }
    while (point != liste->suivant)
    {
        liste = liste->suivant;
    }

    Point *next = liste->suivant->suivant;
    free(liste->suivant);
    liste->suivant = next;
}

void destroy_Cursors(All_Lists *liste)
{
    if (liste->cursor_level.texture != NULL)
        SDL_DestroyTexture(liste->cursor_level.texture);
    if (liste->cursor_tilemap.texture != NULL)
        SDL_DestroyTexture(liste->cursor_tilemap.texture);
    if (liste->cursor_hitbox.texture != NULL)
        SDL_DestroyTexture(liste->cursor_hitbox.texture);
}

void affiche_Cursors(All_Lists *all_liste)
{
    if (all_liste->cursor_level.texture != NULL)
    {
        SDL_RenderCopy(all_liste->renderer, all_liste->cursor_level.texture, NULL, &all_liste->cursor_level.target_render_texture);
    }
    if (all_liste->cursor_tilemap.texture != NULL)
    {
        SDL_RenderCopy(all_liste->renderer, all_liste->cursor_tilemap.texture, NULL, &all_liste->cursor_tilemap.target_render_texture);
    }
}

void affiche_Cursor_hitbox(All_Lists *liste)
{
    if (liste->cursor_hitbox.texture != NULL)
    {
        SDL_RenderCopy(liste->renderer, liste->cursor_hitbox.texture, NULL, &liste->cursor_hitbox.target_render_texture);
    }
}

void destroy_Tilemaps(All_Lists *all_lists)
{
    Tilemap *liste = all_lists->liste_tilemap;
    Tilemap *tmp = NULL;
    while (liste->suivant != NULL)
    {
        SDL_DestroyTexture(liste->suivant->texture);
        tmp = liste->suivant;
        liste->suivant = liste->suivant->suivant;
        free(tmp);
    }
}

void affiche_Tilemap(All_Lists *liste)
{
    SDL_Rect target = {.h = 480, .w = 320, .x = 0, .y = 0};
    if (liste->current_tilemap->texture != NULL)
    {
        SDL_RenderCopy(liste->renderer, liste->current_tilemap->texture, NULL, &target);
    }
}

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
    free(liste->suivant);
    liste->suivant = next;
}

void destroy_Item(Item *item, All_Lists *all_liste)
{
    Item *liste = all_liste->liste_item;
    if (item == NULL)
    {
        printf("Erreur dans destroy_Item : l'item passée en paramètre est un pointeur null \n");
        return;
    }
    while (item != liste->suivant)
    {
        liste = liste->suivant;
    }

    Item *next = liste->suivant->suivant;
    if (liste->suivant->texture != NULL)
    {
        SDL_DestroyTexture(liste->suivant->texture);
        liste->suivant->texture = NULL;
    }
    free(liste->suivant);
    liste->suivant = next;
}

void move_Mob(int x, int y, Mob *mob)
{
    mob->x += x;
    mob->y += y;
    mob->target_render_texture.x += x;
    mob->target_render_texture.y += y;
}

void move_Item(int x, int y, Item *item)
{
    item->x += x;
    item->y += y;
    item->target_render_texture.x += x;
    item->target_render_texture.y += y;
}

void affiche_Mob(Mob *mob, All_Lists *liste)
{
    if (mob->texture != NULL)
    {
        SDL_RenderCopy(liste->renderer, mob->texture, &mob->src_render_texture, &mob->target_render_texture);
    }
}

void affiche_Item(Item *item, All_Lists *liste)
{
    if (item->texture != NULL)
    {
        SDL_RenderCopy(liste->renderer, item->texture, &item->src_render_texture, &item->target_render_texture);
    }
}

void destroy_Level(All_Lists *all_liste)
{
    while (NULL != all_liste->liste_mob->suivant)
        destroy_Mob(all_liste->liste_mob->suivant, all_liste);
    while (NULL != all_liste->liste_item->suivant)
        destroy_Item(all_liste->liste_item->suivant, all_liste);
    while (NULL != all_liste->liste_hitbox->suivant)
        destroy_Hitbox(all_liste->liste_hitbox->suivant, all_liste);
    while (NULL != all_liste->liste_point->suivant)
        destroy_Point(all_liste->liste_point->suivant, all_liste);

    if (all_liste->level.ground != NULL)
    {
        SDL_DestroyTexture(all_liste->level.ground);
        all_liste->level.ground = NULL;
    }
    if (all_liste->level.background != NULL)
    {
        SDL_DestroyTexture(all_liste->level.background);
        all_liste->level.background = NULL;
    }
    if (all_liste->level.background1 != NULL)
    {
        SDL_DestroyTexture(all_liste->level.background1);
        all_liste->level.background1 = NULL;
    }
    if (all_liste->level.background2 != NULL)
    {
        SDL_DestroyTexture(all_liste->level.background2);
        all_liste->level.background2 = NULL;
    }
    if (all_liste->level.background3 != NULL)
    {
        SDL_DestroyTexture(all_liste->level.background3);
        all_liste->level.background3 = NULL;
    }
    if (all_liste->level.background4 != NULL)
    {
        SDL_DestroyTexture(all_liste->level.background4);
        all_liste->level.background4 = NULL;
    }
    if (all_liste->level.current_target != NULL)
    {
        SDL_DestroyTexture(all_liste->level.current_target);
        all_liste->level.current_target = NULL;
    }
    if (all_liste->level.matrice_background != NULL)
    {
        for (int i = 0; i < all_liste->level.size_y; i++)
        {
            free(all_liste->level.matrice_background[i].ligne);
        }
        free(all_liste->level.matrice_background);
    }
    if (all_liste->level.matrice_ground != NULL)
    {
        for (int i = 0; i < all_liste->level.size_y; i++)
        {
            free(all_liste->level.matrice_ground[i].ligne);
        }
        free(all_liste->level.matrice_ground);
    }
    all_liste->level.current_matrice = NULL;
    all_liste->level.size_x = 20;
    all_liste->level.size_y = 15;
    all_liste->cursor_level.x = 0;
    all_liste->cursor_level.y = 0;
    all_liste->cursor_level.target_render_texture.x = 317;
    all_liste->cursor_level.target_render_texture.y = -3;
}

void move_Level(int x, int y, All_Lists *liste)
{
    liste->level.x -= x;
    liste->level.y -= y;
    liste->level.src_render_ground.x += x;
    liste->level.src_render_ground.y += y;
    liste->level.src_render_background1.x = liste->level.src_render_ground.x * 0.75;
    liste->level.src_render_background1.y = liste->level.src_render_ground.y * 0.75;
    liste->level.src_render_background2.x = liste->level.src_render_ground.x / 4;
    liste->level.src_render_background2.y = liste->level.src_render_ground.y / 4;
    liste->level.src_render_background3.x = liste->level.src_render_ground.x / 8;
    liste->level.src_render_background3.y = liste->level.src_render_ground.y / 8;
    liste->level.src_render_background4.x = liste->level.src_render_ground.x / 48;
    liste->level.src_render_background4.y = liste->level.src_render_ground.y / 48;
}

void affiche_Level(All_Lists *liste)
{
    SDL_Rect target = {.h = 240,.w = 320, .x = 320, .y = 0};
    if (liste->level.background4 != NULL)
    {
        SDL_RenderCopy(liste->renderer, liste->level.background4, &liste->level.src_render_background4, &target);
    }
    if (liste->level.background3 != NULL)
    {
        SDL_RenderCopy(liste->renderer, liste->level.background3, &liste->level.src_render_background3, &target);
    }
    if (liste->level.background2 != NULL)
    {
        SDL_RenderCopy(liste->renderer, liste->level.background2, &liste->level.src_render_background2, &target);
    }
    if (liste->level.background1 != NULL)
    {
        SDL_RenderCopy(liste->renderer, liste->level.background1, &liste->level.src_render_background1, &target);
    }
    if (liste->level.background != NULL)
    {
        SDL_RenderCopy(liste->renderer, liste->level.background, &liste->level.src_render_ground, &target);
    }
    if (liste->level.ground != NULL)
    {
        SDL_RenderCopy(liste->renderer, liste->level.ground, &liste->level.src_render_ground, &target);
    }
}

void affiche_Hitbox(Hitbox *hitbox, All_Lists *liste)
{
    int i = 0;
    SDL_SetRenderDrawColor(liste->renderer, 255, 0, 0, 255);
    for (i = 0; i < hitbox->nb_points - 1; i++)
    {
        SDL_RenderDrawLine(liste->renderer, liste->level.x + 320 + hitbox->points[i].x, liste->level.y + hitbox->points[i].y, liste->level.x + 320 + hitbox->points[i+1].x,
                                    liste->level.y + hitbox->points[i+1].y);
    }
    SDL_RenderDrawLine(liste->renderer, liste->level.x + 320 + hitbox->points[i].x, liste->level.y + hitbox->points[i].y, liste->level.x + 320 + hitbox->points[0].x,
                                    liste->level.y + hitbox->points[0].y);
    SDL_SetRenderDrawColor(liste->renderer, 0, 100, 255, 255);
    SDL_RenderDrawLine(liste->renderer, liste->level.x + 320 + hitbox->points[0].x - 3, liste->level.y + hitbox->points[0].y, liste->level.x + 320 + hitbox->points[0].x + 3,
                                    liste->level.y + hitbox->points[0].y);
    SDL_RenderDrawLine(liste->renderer, liste->level.x + 320 + hitbox->points[0].x, liste->level.y + hitbox->points[0].y - 3, liste->level.x + 320 + hitbox->points[0].x,
                                    liste->level.y + hitbox->points[0].y + 3);
    SDL_SetRenderDrawColor(liste->renderer, 0, 0, 0, 255);
}

void affiche_Hitboxes(All_Lists *liste)
{
    Hitbox *hitbox = liste->liste_hitbox;
    while (hitbox->suivant != NULL)
    {
        affiche_Hitbox(hitbox->suivant, liste);
        hitbox = hitbox->suivant;
    }
}

void affiche_Point(Point *point, All_Lists *liste)
{
    SDL_SetRenderDrawColor(liste->renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(liste->renderer, liste->level.x + 320 + point->point.x - 3, liste->level.y + point->point.y, liste->level.x + 320 + point->point.x + 3, liste->level.y + point->point.y);
    SDL_RenderDrawLine(liste->renderer, liste->level.x + 320 + point->point.x, liste->level.y + point->point.y - 3, liste->level.x + 320 + point->point.x, liste->level.y + point->point.y + 3);
    SDL_RenderDrawLine(liste->renderer, liste->level.x + 320 + point->point.x - 3, liste->level.y + point->point.y - 3, liste->level.x + 320 + point->point.x + 3,
                                    liste->level.y + point->point.y + 3);
    SDL_RenderDrawLine(liste->renderer, liste->level.x + 320 + point->point.x + 3, liste->level.y + point->point.y - 3, liste->level.x + 320 + point->point.x - 3,
                                    liste->level.y + point->point.y + 3);
    SDL_SetRenderDrawColor(liste->renderer, 0, 0, 0, 255);
}

void affiche_Points(All_Lists *liste)
{
    Point *point = liste->liste_point;
    while (point->suivant != NULL)
    {
        affiche_Point(point->suivant, liste);
        point = point->suivant;
    }
}

void affiche_help(All_Lists *liste)
{
    if (NULL != liste->help_screen)
        SDL_RenderCopy(liste->renderer, liste->help_screen, NULL, NULL);
}

void affiche_void_background(All_Lists *liste)
{
    if (NULL != liste->void_background)
        SDL_RenderCopy(liste->renderer, liste->void_background, NULL, NULL);
}

void affiche_mode_tilemap_help(All_Lists *liste)
{
    SDL_Rect rect = {.h = 240, .w = 320, .x = 320, .y = 240};
    if (NULL != liste->mode_tilemap_help)
        SDL_RenderCopy(liste->renderer, liste->mode_tilemap_help, NULL, &rect);
}

void affiche_mode_hitbox_help(All_Lists *liste)
{
    if (NULL != liste->mode_hitbox_help)
        SDL_RenderCopy(liste->renderer, liste->mode_hitbox_help, NULL, NULL);
}

void affiche_new_level(All_Lists *liste)
{
    if (NULL != liste->new_level_screen)
        SDL_RenderCopy(liste->renderer, liste->new_level_screen, NULL, NULL);
}

void affiche_hitbox_mode(All_Lists *liste)
{
    SDL_Rect rH = {.x = 0, .y = 0, .h = 60, .w = 320}, rP = {.x = 0, .y = 60, .h = 60, .w = 320}, rAff = {.x = 40, .y = 180, .h = 60, .w = 250};
    if (liste->mode == 3)
    {
        if (NULL != liste->hitbox_mode)
            SDL_RenderCopy(liste->renderer, liste->hitbox_mode, &rH, &rAff);
    }
    else if (liste->mode == 4)
    {
        if (NULL != liste->hitbox_mode)
            SDL_RenderCopy(liste->renderer, liste->hitbox_mode, &rP, &rAff);
    }
}

void affiche_level_mode_target(All_Lists *liste)
{
    SDL_Rect rG = {.x = 0, .y = 0, .h = 60, .w = 320}, rBG = {.x = 0, .y = 60, .h = 60, .w = 320}, rAff = {.x = 360, .y = 420, .h = 60, .w = 250};
    if (liste->level.current_target == liste->level.ground)
    {
        if (NULL != liste->level_mode_target)
            SDL_RenderCopy(liste->renderer, liste->level_mode_target, &rG, &rAff);
    }
    else if (liste->level.current_target == liste->level.background)
    {
        if (NULL != liste->level_mode_target)
            SDL_RenderCopy(liste->renderer, liste->level_mode_target, &rBG, &rAff);
    }
}

void affiche_save_mode_help(All_Lists *liste)
{
    if (NULL != liste->save_mode_help)
        SDL_RenderCopy(liste->renderer, liste->save_mode_help, NULL, NULL);
}

void Quit(All_Lists *liste, int statut)
{
    /* liberation de la RAM allouee */

    destroy_Level(liste);
    destroy_Cursors(liste);
    destroy_Tilemaps(liste);
    if (NULL != liste->help_screen)
        SDL_DestroyTexture(liste->help_screen);
    if (NULL != liste->new_level_screen)
        SDL_DestroyTexture(liste->new_level_screen);
    if (NULL != liste->numbers)
        SDL_DestroyTexture(liste->numbers);
    if (NULL != liste->mode_tilemap_help)
        SDL_DestroyTexture(liste->mode_tilemap_help);
    if (NULL != liste->void_background)
        SDL_DestroyTexture(liste->void_background);
    if (NULL != liste->mode_hitbox_help)
        SDL_DestroyTexture(liste->mode_hitbox_help);
    if (NULL != liste->hitbox_mode)
        SDL_DestroyTexture(liste->hitbox_mode);
    if (NULL != liste->level_mode_target)
        SDL_DestroyTexture(liste->level_mode_target);
    if (NULL != liste->save_mode_help)
        SDL_DestroyTexture(liste->save_mode_help);

    /* destruction du renderer et de la fenetre, fermeture de la SDL puis sortie du programme */

    if (NULL != liste->renderer)
        SDL_DestroyRenderer(liste->renderer);
    if (NULL != liste->window)
        SDL_DestroyWindow(liste->window);
    TTF_Quit();
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

void init_Cursors(All_Lists *all_lists)
{
    all_lists->cursor_level.texture = loadImage("data/cursor.bmp", all_lists->renderer);
    all_lists->cursor_tilemap.texture = loadImage("data/cursor.bmp", all_lists->renderer);
    all_lists->cursor_hitbox.texture = loadImage("data/cursor_hitbox.bmp", all_lists->renderer);
    all_lists->cursor_level.target_render_texture.h = 22;
    all_lists->cursor_level.target_render_texture.w = 22;
    all_lists->cursor_level.target_render_texture.x = 317;
    all_lists->cursor_level.target_render_texture.y = -3;
    all_lists->cursor_level.x = 0;
    all_lists->cursor_level.y = 0;
    all_lists->cursor_level.Id = 1;
    all_lists->cursor_tilemap.target_render_texture.h = 22;
    all_lists->cursor_tilemap.target_render_texture.w = 22;
    all_lists->cursor_tilemap.target_render_texture.x = -3;
    all_lists->cursor_tilemap.target_render_texture.y = -3;
    all_lists->cursor_tilemap.x = 0;
    all_lists->cursor_tilemap.y = 0;
    all_lists->cursor_tilemap.Id = 2;
    all_lists->cursor_hitbox.target_render_texture.h = 22;
    all_lists->cursor_hitbox.target_render_texture.w = 22;
    all_lists->cursor_hitbox.target_render_texture.x = 309;
    all_lists->cursor_hitbox.target_render_texture.y = -11;
    all_lists->cursor_hitbox.x = 0;
    all_lists->cursor_hitbox.y = 0;
    all_lists->cursor_hitbox.Id = 3;
}

void init_Tilemaps(All_Lists *all_lists)
{
    FILE *fichier = NULL;
    int nb_tilemaps;
    Tilemap *tilemap;
    char chemin_tilemap[41];

    fichier = fopen("tilemap/tilemap.dat", "r");
    if (fichier != NULL)
    {
        fscanf(fichier, "%d", &nb_tilemaps);
        for (int i = 0; i < nb_tilemaps; i++)
        {
            fscanf(fichier, " %s", chemin_tilemap);
            tilemap = NULL;
            tilemap = malloc(sizeof(Tilemap));
            tilemap->texture = loadImage(chemin_tilemap, all_lists->renderer);
            tilemap->chemin_tilemap = chemin_tilemap;
            tilemap->suivant = NULL;
            tilemap->precedent = all_lists->current_tilemap;
            all_lists->current_tilemap->suivant = tilemap;
            all_lists->current_tilemap = tilemap;
        }
        if (all_lists->liste_tilemap->suivant != NULL)
            all_lists->current_tilemap = all_lists->liste_tilemap->suivant;
        else
            all_lists->current_tilemap = all_lists->liste_tilemap;
        fclose(fichier);
    }
    else
    {
        printf("L'ouverture du fichier %s a échouée\n", "tilemap/tilemap.dat");
    }
}

void init_Images(All_Lists *all_lists)
{
    all_lists->help_screen = loadImage("data/ecran_d'aide.bmp", all_lists->renderer);
    all_lists->new_level_screen = loadImage("data/new_level_screen.bmp", all_lists->renderer);
    all_lists->numbers = loadImage("data/numbers.bmp", all_lists->renderer);
    all_lists->mode_tilemap_help = loadImage("data/mode_tilemap_help.bmp", all_lists->renderer);
    all_lists->void_background = loadImage("data/void_background.bmp", all_lists->renderer);
    all_lists->mode_hitbox_help = loadImage("data/mode_hitbox_help.bmp", all_lists->renderer);
    all_lists->hitbox_mode = loadImage("data/hitbox_mode.bmp", all_lists->renderer);
    all_lists->level_mode_target = loadImage("data/level_mode_target.bmp", all_lists->renderer);
    all_lists->save_mode_help = loadImage("data/save_mode_help.bmp", all_lists->renderer);
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
    int opp_sides_parallels, Id;

    fichier = fopen(chemin_file, "r");
    if (fichier != NULL)
    {
        fscanf(fichier, "%d %s %d %d %d ", &Id, chemin_image, &opp_sides_parallels, &new_mob->target_render_texture.h, &new_mob->target_render_texture.w);
        fscanf(fichier, "%d %d ", &new_mob->target_render_texture.x, &new_mob->target_render_texture.y);
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

void create_Level(int size_x, int size_y, All_Lists *liste)
{
    destroy_Level(liste);

    liste->level.ground = SDL_CreateTexture(liste->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size_x * 16, size_y * 16);
    liste->level.background = SDL_CreateTexture(liste->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size_x * 16, size_y * 16);
    liste->level.background1 = SDL_CreateTexture(liste->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 320 + ((size_x * 16)-320)*0.75, 240 + ((size_y * 16)-240)*0.75);
    liste->level.background2 = SDL_CreateTexture(liste->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 320 + ((size_x * 16)-320)/4, 240 + ((size_y * 16)-240)/4);
    liste->level.background3 = SDL_CreateTexture(liste->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 320 + ((size_x * 16)-320)/8, 240 + ((size_y * 16)-240)/8);
    liste->level.background4 = SDL_CreateTexture(liste->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 320 + ((size_x * 16)-320)/48, 240 + ((size_y * 16)-240)/48);
    if(NULL == liste->level.ground)
    {
        fprintf(stderr, "Erreur SDL_CreateTexture : %s\n", SDL_GetError());
        return;
    }
    if(NULL == liste->level.background)
    {
        fprintf(stderr, "Erreur SDL_CreateTexture : %s\n", SDL_GetError());
        return;
    }
    if(NULL == liste->level.background1)
    {
        fprintf(stderr, "Erreur SDL_CreateTexture : %s\n", SDL_GetError());
        return;
    }
    if(NULL == liste->level.background2)
    {
        fprintf(stderr, "Erreur SDL_CreateTexture : %s\n", SDL_GetError());
        return;
    }
    if(NULL == liste->level.background3)
    {
        fprintf(stderr, "Erreur SDL_CreateTexture : %s\n", SDL_GetError());
        return;
    }
    if(NULL == liste->level.background4)
    {
        fprintf(stderr, "Erreur SDL_CreateTexture : %s\n", SDL_GetError());
        return;
    }
    SDL_SetTextureBlendMode(liste->level.ground, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(liste->level.background, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(liste->level.background1, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(liste->level.background2, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(liste->level.background3, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(liste->level.background4, SDL_BLENDMODE_BLEND);

    char chemin_tilemap[41];
    Lign_of_Tiles matrice[size_y];
    Tile ligne[size_x];
    liste->level.matrice_background = malloc(sizeof(matrice));
    liste->level.matrice_ground = malloc(sizeof(matrice));
    for (int i = 0; i < size_y; i++)
    {
        liste->level.matrice_background[i].ligne = malloc(sizeof(ligne));
        liste->level.matrice_ground[i].ligne = malloc(sizeof(ligne));
        for (int j = 0; j < size_x; j++)
        {
            liste->level.matrice_background[i].ligne[j].tilemap = "none";
            liste->level.matrice_ground[i].ligne[j].tilemap = "none";
            liste->level.matrice_background[i].ligne[j].x = 0;
            liste->level.matrice_background[i].ligne[j].y = 0;
            liste->level.matrice_ground[i].ligne[j].x = 0;
            liste->level.matrice_ground[i].ligne[j].y = 0;
        }
    }
    liste->level.current_matrice = liste->level.matrice_ground;
    liste->level.current_target = liste->level.ground;
    liste->level.size_x = size_x;
    liste->level.size_y = size_y;
    liste->level.x = 0;
    liste->level.y = 0;
    liste->level.src_render_ground.x = 0;
    liste->level.src_render_ground.y = 0;
    liste->level.src_render_background1.x = 0;
    liste->level.src_render_background1.y = 0;
    liste->level.src_render_background2.x = 0;
    liste->level.src_render_background2.y = 0;
    liste->level.src_render_background3.x = 0;
    liste->level.src_render_background3.y = 0;
    liste->level.src_render_background4.x = 0;
    liste->level.src_render_background4.y = 0;
    liste->level.src_render_ground.w = 320;
    liste->level.src_render_ground.h = 240;
    liste->level.src_render_background1.w = 320;
    liste->level.src_render_background1.h = 240;
    liste->level.src_render_background2.w = 320;
    liste->level.src_render_background2.h = 240;
    liste->level.src_render_background3.w = 320;
    liste->level.src_render_background3.h = 240;
    liste->level.src_render_background4.w = 320;
    liste->level.src_render_background4.h = 240;
}

void Scroll(int x, int y, All_Lists *liste)
{
    move_Level(x, y, liste);
    Mob *mob_cible = liste->liste_mob->suivant;
    Item *item_cible = liste->liste_item->suivant;
    while (mob_cible != NULL)
    {
        move_Mob(x, y, mob_cible);
        mob_cible = mob_cible->suivant;
    }
    while (item_cible != NULL)
    {
        move_Item(x, y, item_cible);
        item_cible = item_cible->suivant;
    }
}

void move_Cursor(int x, int y, Cursor *cursor, All_Lists *liste)
{
    if (cursor->Id == 1)
    {
        if (622 > cursor->target_render_texture.x + x*16 && cursor->target_render_texture.x + x*16 > 316)
        {
            cursor->x += x;
            cursor->target_render_texture.x += x*16;
        }
        else if (liste->level.size_x > cursor->x + x && cursor->x + x >= 0)
        {
            cursor->x += x;
            Scroll(x * 16, 0, liste);
        }
        if (222 > cursor->target_render_texture.y + y*16 && cursor->target_render_texture.y + y*16 > -4)
        {
            cursor->y += y;
            cursor->target_render_texture.y += y*16;
        }
        else if (liste->level.size_y > cursor->y + y && cursor->y + y >= 0)
        {
            cursor->y += y;
            Scroll(0, y * 16, liste);
        }
    }

    if (cursor->Id == 2)
    {
        if (302 > cursor->target_render_texture.x + x*16 && cursor->target_render_texture.x + x*16 > -4)
        {
            cursor->x += x;
            cursor->target_render_texture.x += x*16;
        }
        if (462 > cursor->target_render_texture.y + y*16 && cursor->target_render_texture.y + y*16 > -4)
        {
            cursor->y += y;
            cursor->target_render_texture.y += y*16;
        }
    }
    
    if (cursor->Id == 3)
    {
        if (630 > cursor->target_render_texture.x + x*16 && cursor->target_render_texture.x + x*16 > 308)
        {
            cursor->x += x;
            cursor->target_render_texture.x += x*16;
        }
        else if (liste->level.size_x + 1 > cursor->x + x && cursor->x + x >= 0)
        {
            cursor->x += x;
            Scroll(x * 16, 0, liste);
        }
        if (230 > cursor->target_render_texture.y + y*16 && cursor->target_render_texture.y + y*16 > -12)
        {
            cursor->y += y;
            cursor->target_render_texture.y += y*16;
        }
        else if (liste->level.size_y + 1 > cursor->y + y && cursor->y + y >= 0)
        {
            cursor->y += y;
            Scroll(0, y * 16, liste);
        }
    }
}

void build_Hitbox(All_Lists *liste)
{
    Point *point = liste->liste_point;
    Hitbox *hitbox = liste->liste_hitbox;
    int nb_points = 0, x_min = 0, x_max = 0, y_min = 0, y_max = 0;
    while (hitbox->suivant != NULL)
    {
        hitbox = hitbox->suivant;
    }
    while (point->suivant != NULL)
    {
        nb_points += 1;
        point = point->suivant;
    }
    if (3 > nb_points)
    {
        point = liste->liste_point;
        while (point->suivant != NULL)
            destroy_Point(point->suivant, liste);
        return;
    }
    point = liste->liste_point->suivant;
    Hitbox *new_hitbox = malloc(sizeof(Hitbox));
    hitbox->suivant = new_hitbox;
    SDL_Point new_point[nb_points];
    new_hitbox->points = malloc(sizeof(new_point));
    new_hitbox->nb_points = nb_points;
    new_hitbox->suivant = NULL;
    if (new_hitbox->points == NULL)
    {
        Quit(liste, EXIT_FAILURE);
    }
    Point *tmp = NULL;
    for (int i = 0; i < new_hitbox->nb_points; i++)
    {
        new_hitbox->points[i].x = point->point.x;
        new_hitbox->points[i].y = point->point.y;
        if (x_min > point->point.x)
            x_min = point->point.x;
        if (point->point.x > x_max)
            x_max = point->point.x;
        if (y_min > point->point.y)
            y_min = point->point.y;
        if (point->point.y > y_max)
            y_max = point->point.y;
        tmp = point;
        point = point->suivant;
        destroy_Point(tmp, liste);
    }
    new_hitbox->cercle_x = x_min + (x_max - x_min) / 2;
    new_hitbox->cercle_y = y_min + (y_max - y_min) / 2;
    new_hitbox->cercle_rayon = (int)sqrt((double)((x_max-x_min) * (x_max-x_min) + (y_max-y_min) * (y_max-y_min))) / 2;
    new_hitbox->opp_sides_parallels = SDL_TRUE;
    int i = 0;
    if (new_hitbox->nb_points % 2 == 0)
    {
        for (i = 0; i < new_hitbox->nb_points / 2 - 1; i++)
        {
            if (new_hitbox->points[i].x - new_hitbox->points[i+1].x != new_hitbox->points[i + new_hitbox->nb_points / 2 + 1].x - new_hitbox->points[i + new_hitbox->nb_points / 2].x)
            {
                new_hitbox->opp_sides_parallels = SDL_FALSE;
                return;
            }
            if (new_hitbox->points[i].y - new_hitbox->points[i+1].y != new_hitbox->points[i + new_hitbox->nb_points / 2 + 1].y - new_hitbox->points[i + new_hitbox->nb_points / 2].y)
            {
                new_hitbox->opp_sides_parallels = SDL_FALSE;
                return;
            }
        }
        if (new_hitbox->points[i+1].x - new_hitbox->points[i+2].x != new_hitbox->points[0].x - new_hitbox->points[i + new_hitbox->nb_points / 2 + 1].x)
        {
            new_hitbox->opp_sides_parallels = SDL_FALSE;
            return;
        }
        if (new_hitbox->points[i+1].y - new_hitbox->points[i+2].y != new_hitbox->points[0].y - new_hitbox->points[i + new_hitbox->nb_points / 2 + 1].y)
        {
            new_hitbox->opp_sides_parallels = SDL_FALSE;
            return;
        }
    }
    else
    {
        new_hitbox->opp_sides_parallels = SDL_FALSE;
    }
}

void change_Tilemap(Input *input, All_Lists *all_lists)
{
    if (input->key[SDL_GetScancodeFromKey(SDLK_c)])
    {
        if (all_lists->current_tilemap->precedent != NULL && all_lists->current_tilemap->precedent->texture != NULL)
        {
            all_lists->current_tilemap = all_lists->current_tilemap->precedent;
        }
        input->key[SDL_GetScancodeFromKey(SDLK_c)] = SDL_FALSE;
    }
    if (input->key[SDL_GetScancodeFromKey(SDLK_f)])
    {
        if (all_lists->current_tilemap->suivant != NULL && all_lists->current_tilemap->suivant->texture != NULL)
        {
            all_lists->current_tilemap = all_lists->current_tilemap->suivant;
        }
        input->key[SDL_GetScancodeFromKey(SDLK_f)] = SDL_FALSE;
    }
}

void update_Cursors(Input *input, All_Lists *all_lists)
{
    if (all_lists->mode == 1)
    {
        if (input->key[SDL_GetScancodeFromKey(SDLK_q)])
        {   
            move_Cursor(-1, 0, &all_lists->cursor_tilemap, all_lists);
            input->key[SDL_GetScancodeFromKey(SDLK_q)] = SDL_FALSE;
        }
        if (input->key[SDL_GetScancodeFromKey(SDLK_d)])
        {
            move_Cursor(1, 0, &all_lists->cursor_tilemap, all_lists);
            input->key[SDL_GetScancodeFromKey(SDLK_d)] = SDL_FALSE;
        }
        if (input->key[SDL_GetScancodeFromKey(SDLK_z)])
        {
            move_Cursor(0, -1, &all_lists->cursor_tilemap, all_lists);
            input->key[SDL_GetScancodeFromKey(SDLK_z)] = SDL_FALSE;
        }
        if (input->key[SDL_GetScancodeFromKey(SDLK_s)])
        {
            move_Cursor(0, 1, &all_lists->cursor_tilemap, all_lists);
            input->key[SDL_GetScancodeFromKey(SDLK_s)] = SDL_FALSE;
        }
        if (input->key[SDL_GetScancodeFromKey(SDLK_k)])
        {
            move_Cursor(-1, 0, &all_lists->cursor_level, all_lists);
            input->key[SDL_GetScancodeFromKey(SDLK_k)] = SDL_FALSE;
        }
        if (input->key[SDL_GetScancodeFromKey(SDLK_m)])
        {
            move_Cursor(1, 0, &all_lists->cursor_level, all_lists);
            input->key[SDL_GetScancodeFromKey(SDLK_m)] = SDL_FALSE;
        }
        if (input->key[SDL_GetScancodeFromKey(SDLK_o)])
        {
            move_Cursor(0, -1, &all_lists->cursor_level, all_lists);
            input->key[SDL_GetScancodeFromKey(SDLK_o)] = SDL_FALSE;
        }
        if (input->key[SDL_GetScancodeFromKey(SDLK_l)])
        {
            move_Cursor(0, 1, &all_lists->cursor_level, all_lists);
            input->key[SDL_GetScancodeFromKey(SDLK_l)] = SDL_FALSE;
        }
    }
    else if (all_lists->mode == 3 || all_lists->mode == 4)
    {
        if (input->key[SDL_GetScancodeFromKey(SDLK_k)])
        {
            move_Cursor(-1, 0, &all_lists->cursor_hitbox, all_lists);
            input->key[SDL_GetScancodeFromKey(SDLK_k)] = SDL_FALSE;
        }
        if (input->key[SDL_GetScancodeFromKey(SDLK_m)])
        {
            move_Cursor(1, 0, &all_lists->cursor_hitbox, all_lists);
            input->key[SDL_GetScancodeFromKey(SDLK_m)] = SDL_FALSE;
        }
        if (input->key[SDL_GetScancodeFromKey(SDLK_o)])
        {
            move_Cursor(0, -1, &all_lists->cursor_hitbox, all_lists);
            input->key[SDL_GetScancodeFromKey(SDLK_o)] = SDL_FALSE;
        }
        if (input->key[SDL_GetScancodeFromKey(SDLK_l)])
        {
            move_Cursor(0, 1, &all_lists->cursor_hitbox, all_lists);
            input->key[SDL_GetScancodeFromKey(SDLK_l)] = SDL_FALSE;
        }
    }
}

void save_Level(All_Lists *liste)
{
    return;
}

void update_mode(Input *input, All_Lists *all_lists)
{
    if (all_lists->mode == 0)    /* l'editeur est en mode help */
    {
        if (input->key[SDL_GetScancodeFromKey(SDLK_h)])
        {
            all_lists->mode = 1;
            all_lists->cursor_hitbox.x = all_lists->cursor_level.x;
            all_lists->cursor_hitbox.y = all_lists->cursor_level.y;
            all_lists->cursor_hitbox.target_render_texture.x = all_lists->cursor_level.target_render_texture.x - 8;
            all_lists->cursor_hitbox.target_render_texture.y = all_lists->cursor_level.target_render_texture.y - 8;
            input->key[SDL_GetScancodeFromKey(SDLK_h)] = SDL_FALSE;
        }
        else if (input->key[SDL_GetScancodeFromKey(SDLK_n)])
        {
            all_lists->mode = 2;
            destroy_Level(all_lists);
            input->key[SDL_GetScancodeFromKey(SDLK_n)] = SDL_FALSE;
        }
        else if (input->key[SDL_GetScancodeFromKey(SDLK_t)])
        {
            all_lists->mode = 3;
            all_lists->cursor_hitbox.x = all_lists->cursor_level.x;
            all_lists->cursor_hitbox.y = all_lists->cursor_level.y;
            all_lists->cursor_hitbox.target_render_texture.x = all_lists->cursor_level.target_render_texture.x - 8;
            all_lists->cursor_hitbox.target_render_texture.y = all_lists->cursor_level.target_render_texture.y - 8;
            input->key[SDL_GetScancodeFromKey(SDLK_t)] = SDL_FALSE;
        }
        else if (input->key[SDL_GetScancodeFromKey(SDLK_g)])
        {
            all_lists->mode = 5;
            input->key[SDL_GetScancodeFromKey(SDLK_g)] = SDL_FALSE;
        }
    }
    else if (all_lists->mode == 1)   /* l'editeur est en mode tilemap */
    {
        if (input->key[SDL_GetScancodeFromKey(SDLK_h)])
        {
            all_lists->mode = 0;
            input->key[SDL_GetScancodeFromKey(SDLK_h)] = SDL_FALSE;
        }
        else if (input->key[SDL_GetScancodeFromKey(SDLK_n)])
        {
            all_lists->mode = 2;
            destroy_Level(all_lists);
            input->key[SDL_GetScancodeFromKey(SDLK_n)] = SDL_FALSE;
        }
        else if (input->key[SDL_GetScancodeFromKey(SDLK_t)])
        {
            all_lists->mode = 3;
            all_lists->cursor_hitbox.x = all_lists->cursor_level.x;
            all_lists->cursor_hitbox.y = all_lists->cursor_level.y;
            all_lists->cursor_hitbox.target_render_texture.x = all_lists->cursor_level.target_render_texture.x - 8;
            all_lists->cursor_hitbox.target_render_texture.y = all_lists->cursor_level.target_render_texture.y - 8;
            input->key[SDL_GetScancodeFromKey(SDLK_t)] = SDL_FALSE;
        }
        else if (input->key[SDL_GetScancodeFromKey(SDLK_g)])
        {
            all_lists->mode = 5;
            input->key[SDL_GetScancodeFromKey(SDLK_g)] = SDL_FALSE;
        }
    }
    else if (all_lists->mode == 2)   /* l'editeur est en mode creation d'un nouveau niveau */
    {
        if (input->key[SDL_GetScancodeFromKey(SDLK_SPACE)])
        {
            create_Level(all_lists->level.size_x, all_lists->level.size_y, all_lists);
            all_lists->mode = 1;
            input->key[SDL_GetScancodeFromKey(SDLK_SPACE)] = SDL_FALSE;
        }
    }
    else if (all_lists->mode == 3)   /* l'editeur est en mode hitbox */
    {
        if (input->key[SDL_GetScancodeFromKey(SDLK_h)])
        {
            all_lists->mode = 0;
            all_lists->cursor_level.x = all_lists->cursor_hitbox.x;
            all_lists->cursor_level.y = all_lists->cursor_hitbox.y;
            all_lists->cursor_level.target_render_texture.x = all_lists->cursor_hitbox.target_render_texture.x + 8;
            all_lists->cursor_level.target_render_texture.y = all_lists->cursor_hitbox.target_render_texture.y + 8;
            input->key[SDL_GetScancodeFromKey(SDLK_h)] = SDL_FALSE;
        }
        else if (input->key[SDL_GetScancodeFromKey(SDLK_n)])
        {
            all_lists->mode = 2;
            destroy_Level(all_lists);
            input->key[SDL_GetScancodeFromKey(SDLK_n)] = SDL_FALSE;
        }
        else if (input->key[SDL_GetScancodeFromKey(SDLK_t)])
        {
            all_lists->mode = 1;
            all_lists->cursor_level.x = all_lists->cursor_hitbox.x;
            all_lists->cursor_level.y = all_lists->cursor_hitbox.y;
            all_lists->cursor_level.target_render_texture.x = all_lists->cursor_hitbox.target_render_texture.x + 8;
            all_lists->cursor_level.target_render_texture.y = all_lists->cursor_hitbox.target_render_texture.y + 8;
            input->key[SDL_GetScancodeFromKey(SDLK_t)] = SDL_FALSE;
        }
        else if (input->key[SDL_GetScancodeFromKey(SDLK_z)])
        {
            all_lists->mode = 4;
            input->key[SDL_GetScancodeFromKey(SDLK_z)] = SDL_FALSE;
        }
        else if (input->key[SDL_GetScancodeFromKey(SDLK_g)])
        {
            all_lists->mode = 5;
            all_lists->cursor_level.x = all_lists->cursor_hitbox.x;
            all_lists->cursor_level.y = all_lists->cursor_hitbox.y;
            all_lists->cursor_level.target_render_texture.x = all_lists->cursor_hitbox.target_render_texture.x + 8;
            all_lists->cursor_level.target_render_texture.y = all_lists->cursor_hitbox.target_render_texture.y + 8;
            input->key[SDL_GetScancodeFromKey(SDLK_g)] = SDL_FALSE;
        }
    }
    else if (all_lists->mode == 4)  /* l'editeur est en mode hitbox et est en cours de creation d'une nouvelle hitbox */
    {
        if (input->key[SDL_GetScancodeFromKey(SDLK_z)])
        {
            all_lists->mode = 3;
            build_Hitbox(all_lists);
            input->key[SDL_GetScancodeFromKey(SDLK_z)] = SDL_FALSE;
        }
    }
    else if (all_lists->mode == 5)  /* l'editeur est en mode sauvegarde du niveau */
    {
        if (input->key[SDL_GetScancodeFromKey(SDLK_h)])
        {
            all_lists->mode = 0;
            input->key[SDL_GetScancodeFromKey(SDLK_h)] = SDL_FALSE;
        }
        if (input->key[SDL_GetScancodeFromKey(SDLK_s)])
        {
            input->key[SDL_GetScancodeFromKey(SDLK_s)] = SDL_FALSE;
            save_Level(all_lists);
            all_lists->mode = 0;
        }
    }
}

void efface_tile(Input *input, All_Lists *all_lists)
{
    SDL_Rect rect_cursor_level = {.h = 16, .w = 16, .x = all_lists->cursor_level.x * 16, .y = all_lists->cursor_level.y * 16};
    SDL_Texture *tmp = NULL;
    if (input->key[SDL_GetScancodeFromKey(SDLK_e)])
    {
        if (all_lists->current_tilemap->texture != NULL)
        {
            if (all_lists->level.current_target != NULL)
            {
                tmp = SDL_CreateTexture(all_lists->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 16, 16);
                SDL_SetRenderTarget(all_lists->renderer, all_lists->level.current_target);
                SDL_RenderCopy(all_lists->renderer, tmp, NULL, &rect_cursor_level);
                SDL_SetRenderTarget(all_lists->renderer, NULL);
                all_lists->level.current_matrice[all_lists->cursor_level.y].ligne[all_lists->cursor_level.x].tilemap = "none";
                all_lists->level.current_matrice[all_lists->cursor_level.y].ligne[all_lists->cursor_level.x].x = 0;
                all_lists->level.current_matrice[all_lists->cursor_level.y].ligne[all_lists->cursor_level.x].y = 0;
            }
        }
    }
    if (NULL != tmp)
        SDL_DestroyTexture(tmp);
}

void copie_colle_tile(Input *input, All_Lists *all_lists)
{
    SDL_Rect rect_cursor_tilemap = {.h = 16, .w = 16, .x = all_lists->cursor_tilemap.x * 16, .y = all_lists->cursor_tilemap.y * 16};
    SDL_Rect rect_cursor_level = {.h = 16, .w = 16, .x = all_lists->cursor_level.x * 16, .y = all_lists->cursor_level.y * 16};
    SDL_Texture *tmp = NULL;
    if (input->key[SDL_GetScancodeFromKey(SDLK_SPACE)])
    {
        if (all_lists->current_tilemap->texture != NULL)
        {
            if (all_lists->level.current_target != NULL)
            {
                tmp = SDL_CreateTexture(all_lists->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 16, 16);
                SDL_SetRenderTarget(all_lists->renderer, all_lists->level.current_target);
                SDL_RenderCopy(all_lists->renderer, tmp, NULL, &rect_cursor_level);
                SDL_RenderCopy(all_lists->renderer, all_lists->current_tilemap->texture, &rect_cursor_tilemap, &rect_cursor_level);
                SDL_SetRenderTarget(all_lists->renderer, NULL);
                all_lists->level.current_matrice[all_lists->cursor_level.y].ligne[all_lists->cursor_level.x].tilemap = all_lists->current_tilemap->chemin_tilemap;
                all_lists->level.current_matrice[all_lists->cursor_level.y].ligne[all_lists->cursor_level.x].x = all_lists->cursor_tilemap.x;
                all_lists->level.current_matrice[all_lists->cursor_level.y].ligne[all_lists->cursor_level.x].y = all_lists->cursor_tilemap.y;
            }
        }
    }
    if (NULL != tmp)
        SDL_DestroyTexture(tmp);
}

void change_edition_target(Input *input, All_Lists *all_lists)
{
    if (input->key[SDL_GetScancodeFromKey(SDLK_r)])
    {
        if (all_lists->level.current_target == all_lists->level.ground)
        {
            all_lists->level.current_target = all_lists->level.background;
            all_lists->level.current_matrice = all_lists->level.matrice_background;
        }
        else
        {
            all_lists->level.current_target = all_lists->level.ground;
            all_lists->level.current_matrice = all_lists->level.matrice_ground;
        }
        input->key[SDL_GetScancodeFromKey(SDLK_r)] = SDL_FALSE;
    }
}

void update_new_level_size(Input *input, All_Lists *all_lists)
{
    if (input->key[SDL_GetScancodeFromKey(SDLK_z)])
    {
        if (all_lists->level.size_y - 1 > -1)
            all_lists->level.size_y -= 1;
        input->key[SDL_GetScancodeFromKey(SDLK_z)] = SDL_FALSE;
    }
    if (input->key[SDL_GetScancodeFromKey(SDLK_s)])
    {
        if (1000 > all_lists->level.size_y + 1)
            all_lists->level.size_y += 1;
        input->key[SDL_GetScancodeFromKey(SDLK_s)] = SDL_FALSE;
    }
    if (input->key[SDL_GetScancodeFromKey(SDLK_q)])
    {
        if (all_lists->level.size_x - 1 > -1)
            all_lists->level.size_x -= 1;
        input->key[SDL_GetScancodeFromKey(SDLK_q)] = SDL_FALSE;
    }
    if (input->key[SDL_GetScancodeFromKey(SDLK_d)])
    {
        if (1000 > all_lists->level.size_x + 1)
            all_lists->level.size_x += 1;
        input->key[SDL_GetScancodeFromKey(SDLK_d)] = SDL_FALSE;
    }

    SDL_Rect rsx = {.h = 16, .w = 64, .x = 0, .y = all_lists->level.size_x * 16};
    SDL_Rect rtx = {.h = 16, .w = 64, .x = 180, .y = 173};
    SDL_Rect rsy = {.h = 16, .w = 64, .x = 0, .y = all_lists->level.size_y * 16};
    SDL_Rect rty = {.h = 16, .w = 64, .x = 180, .y = 210};
    if (NULL != all_lists->numbers)
    {
        SDL_RenderCopy(all_lists->renderer, all_lists->numbers, &rsx, &rtx);
        SDL_RenderCopy(all_lists->renderer, all_lists->numbers, &rsy, &rty);
    }
}

void ajoute_point(Input *input, All_Lists *all_lists)
{
    Point *liste = all_lists->liste_point;
    if (input->key[SDL_GetScancodeFromKey(SDLK_SPACE)])
    {
        input->key[SDL_GetScancodeFromKey(SDLK_SPACE)] = SDL_FALSE;
        while (liste->suivant != NULL)
        {
            liste = liste->suivant;
        }
        liste->suivant = malloc(sizeof(Point));
        liste = liste->suivant;
        liste->suivant = NULL;
        liste->point.x = all_lists->cursor_hitbox.x * 16;
        liste->point.y = all_lists->cursor_hitbox.y * 16;
    }
}

void supprime_point(Input *input, All_Lists *all_lists)
{
    Point *liste = all_lists->liste_point->suivant;
    if (input->key[SDL_GetScancodeFromKey(SDLK_d)])
    {
        input->key[SDL_GetScancodeFromKey(SDLK_d)] = SDL_FALSE;
        if (liste == NULL)
                return;
        while (!(liste->point.x == all_lists->cursor_hitbox.x * 16 && liste->point.y == all_lists->cursor_hitbox.y * 16))
        {
            if (liste->suivant == NULL)
                return;
            liste = liste->suivant;
        }
        destroy_Point(liste, all_lists);
    }
}

void supprime_hitbox(Input *input, All_Lists *all_lists)
{
    Hitbox *liste = all_lists->liste_hitbox->suivant;
    if (input->key[SDL_GetScancodeFromKey(SDLK_d)])
    {
        input->key[SDL_GetScancodeFromKey(SDLK_d)] = SDL_FALSE;
        if (liste == NULL)
                return;
        while (!(liste->points[0].x == all_lists->cursor_hitbox.x * 16 && liste->points[0].y == all_lists->cursor_hitbox.y * 16))
        {
            if (liste->suivant == NULL)
                return;
            liste = liste->suivant;
        }
        destroy_Hitbox(liste, all_lists);
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

    liste->window = SDL_CreateWindow("Metroidvania Level Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              320*20, 240*20, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
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
    SDL_RenderSetLogicalSize(liste->renderer, 320*2, 240*2);

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
    if (0 != TTF_Init())
    {
        fprintf(stderr, "Erreur TTF_Init : %s\n", SDL_GetError());
        Quit(liste, EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    /* Création des variables */

    Item liste_item = {.suivant = NULL};
    Mob liste_mob = {.suivant = NULL};
    Hitbox liste_hitbox = {.suivant = NULL};
    Point liste_point = {.suivant = NULL};
    Tilemap liste_tilemap = {.chemin_tilemap = NULL, .suivant = NULL, .precedent = NULL, .texture = NULL};
    All_Lists all_lists = {.liste_tilemap = &liste_tilemap, .current_tilemap = &liste_tilemap, .liste_item = &liste_item, .liste_mob = &liste_mob,
                                            .liste_hitbox = &liste_hitbox, .liste_point = &liste_point, .mode = 0, .renderer = NULL, .window = NULL};
    Input input = {.quit = SDL_FALSE};
    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
        input.key[i] = SDL_FALSE;

    /* Initialisations, création de la fenêtre et du renderer. */

    Init(&all_lists);
    init_Cursors(&all_lists);
    init_Tilemaps(&all_lists);
    init_Images(&all_lists);

    /* Boucle principale du jeu */

    while (!input.quit)
    {
        updateEvent(&input);
        SDL_RenderClear(all_lists.renderer);

        if (all_lists.mode == 0)    /* l'editeur est en mode help */
        {
            update_mode(&input, &all_lists);
            affiche_help(&all_lists);
        }

        else if (all_lists.mode == 1)   /* l'editeur est en mode tilemap */
        {
            update_mode(&input, &all_lists);
            change_Tilemap(&input, &all_lists);
            change_edition_target(&input, &all_lists);
            efface_tile(&input, &all_lists);
            copie_colle_tile(&input, &all_lists);
            update_Cursors(&input, &all_lists);

            affiche_void_background(&all_lists);
            affiche_mode_tilemap_help(&all_lists);
            affiche_level_mode_target(&all_lists);
            affiche_Tilemap(&all_lists);
            affiche_Level(&all_lists);
            affiche_Cursors(&all_lists);
        }

        else if (all_lists.mode == 2)   /* l'editeur est en mode creation d'un nouveau niveau */
        {
            affiche_new_level(&all_lists);
            update_new_level_size(&input, &all_lists);
            update_mode(&input, &all_lists);
        }

        else if (all_lists.mode == 3)   /* l'editeur est en mode hitbox */
        {
            supprime_hitbox(&input, &all_lists);
            update_mode(&input, &all_lists);
            update_Cursors(&input, &all_lists);

            affiche_void_background(&all_lists);
            affiche_Level(&all_lists);
            affiche_Hitboxes(&all_lists);
            affiche_mode_hitbox_help(&all_lists);
            affiche_hitbox_mode(&all_lists);
            affiche_Cursor_hitbox(&all_lists);
        }
        else if (all_lists.mode == 4)   /* l'editeur est en mode hitbox et est en cours de creation d'une nouvelle hitbox */
        {
            ajoute_point(&input, &all_lists);
            supprime_point(&input, &all_lists);
            update_mode(&input, &all_lists);
            update_Cursors(&input, &all_lists);

            affiche_void_background(&all_lists);
            affiche_Level(&all_lists);
            affiche_Hitboxes(&all_lists);
            affiche_Points(&all_lists);
            affiche_mode_hitbox_help(&all_lists);
            affiche_hitbox_mode(&all_lists);
            affiche_Cursor_hitbox(&all_lists);
        }
        else if (all_lists.mode == 5)   /* l'editeur est en mode sauvegarde du niveau */
        {
            update_mode(&input, &all_lists);
            affiche_save_mode_help(&all_lists);
        }
        
        SDL_RenderPresent(all_lists.renderer);
        SDL_Delay( (int)(1000 / 60) );
    }

    /* Fermeture du logiciel et libération de la mémoire */

    Quit(&all_lists, EXIT_SUCCESS);
}