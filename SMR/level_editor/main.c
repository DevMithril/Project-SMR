#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../lib/level.h"
#include "input.h"
#include "texture.h"

typedef enum Mode
{
    SETUP,
    TILEMAP,
    LEVEL,
    HITBOX
}Mode;

typedef struct Cursor
{
    SDL_Texture *texture_active, *texture_inactive;
    SDL_Rect dst, tile_src_dst;
}Cursor;

typedef struct Everything
{
    Level level;
    Input input;
    Cursor cur_hitbox, cur_tilemap, cur_level;
    Hitbox hitbox_edit;
    Tile *tile_array;
    SDL_Texture **tilemap_array;
    int nb_tiles, cur_tilemap_indice, cur_point_indice;
    Mode mode;
    SDL_Renderer *renderer;
    SDL_Window *window;
}Everything;

void destroy_Cursor(Cursor *cursor)
{
    if (cursor->texture_active != NULL)
    {
        SDL_DestroyTexture(cursor->texture_active);
    }
    if (cursor->texture_inactive != NULL)
    {
        SDL_DestroyTexture(cursor->texture_inactive);
    }
}

void quit(Everything *all, int status)
{
    /* liberation de la RAM allouee */

    destroy_Level(&all->level);
    destroy_Cursor(&all->cur_hitbox);
    destroy_Cursor(&all->cur_level);
    destroy_Cursor(&all->cur_tilemap);
    if (all->tile_array != NULL)
    {
        free(all->tile_array);
    }
    destroy_Tilemaps(all->tilemap_array);

    /* destruction du renderer et de la fenetre, fermetures de la TTF et de la SDL puis sortie du programme */

    if (NULL != all->renderer)
        SDL_DestroyRenderer(all->renderer);
    if (NULL != all->window)
        SDL_DestroyWindow(all->window);
    TTF_Quit();
    SDL_Quit();
    printf("program returns code : %d\n", status);
    exit(status);
}

void display_LevelEdit(Everything *all)
{
    SDL_Rect src, dst;
    for (int i = 0; i < all->nb_tiles; i++)
    {
        src = all->tile_array[i].src;
        dst = all->tile_array[i].dst;
        dst.x += 320 - all->level.src.x;
        dst.y -= all->level.src.y;
        if (dst.x >= 320 && dst.x <= 2*320 - 16 && dst.y >= 0 && dst.y <= 240 - 16)
        {
            SDL_RenderCopy(all->renderer, all->tilemap_array[all->tile_array[i].tilemap_id], &src, &dst);
        }
    }
}

void init(Everything *all, const char *window_name, const char *icon_path, int res_x, int res_y, SDL_bool fullscreen)
{
    /* initialisation de la SDL et de la TTF */

    if (SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "Erreur SDL_Init : %s\n", SDL_GetError());
        quit(all, EXIT_FAILURE);
    }
    if (TTF_Init())
    {
        fprintf(stderr, "Erreur TTF_Init : %s\n", TTF_GetError());
        quit(all, EXIT_FAILURE);
    }

    /* creation de la fenetre et du renderer associé */

    if (fullscreen)
    {
        all->window = SDL_CreateWindow(window_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                10*res_x, 10*res_y, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
    }
    else
    {
        all->window = SDL_CreateWindow(window_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                2*res_x, 2*res_y, SDL_WINDOW_SHOWN);
    }
    if (NULL == all->window)
    {
        fprintf(stderr, "Erreur SDL_CreateWindow : %s\n", SDL_GetError());
        quit(all, EXIT_FAILURE);
    }
    all->renderer = SDL_CreateRenderer(all->window, -1, SDL_RENDERER_ACCELERATED);
    if (NULL == all->renderer)
    {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s\n", SDL_GetError());
        quit(all, EXIT_FAILURE);
    }
    SDL_RenderSetLogicalSize(all->renderer, res_x, res_y);

    /* chargement et affichage de l'icone de la fenetre */
    
    if (!strcmp(icon_path, "none"))
    {
        return;
    }
    SDL_Surface *icone;
    icone = SDL_LoadBMP(icon_path);
    if (NULL == icone)
    {
        fprintf(stderr, "Erreur SDL_LoadBMP : %s\n", SDL_GetError());
        quit(all, EXIT_FAILURE);
    }
    SDL_SetWindowIcon(all->window, icone);
    SDL_FreeSurface(icone);
}

void load_level(Everything *all)
{
    char file_path[200];
    FILE *file = NULL;

    printf("Ecrire le chemin du fichier à charger ('none' pour créer un nouveau) : ");
    scanf(" %s", file_path);
    if (!strcmp(file_path, "none"))
    {
        return;
    }

    file = fopen(file_path, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "load_level : impossible d'ouvrir ou de créer un fichier '%s'\n", file_path);
        return;
    }

    fread(&all->level, sizeof(Level), 1, file);
    fread(&all->nb_tiles, sizeof(int), 1, file);
    all->tile_array =  malloc(sizeof(Tile) * all->nb_tiles);
    if (all->tile_array == NULL && all->nb_tiles != 0)
    {
        fprintf(stderr, "load_level : Out of Memory\n");
        return;
    }
    fread(all->tile_array, sizeof(Tile), all->nb_tiles, file);
    all->level.hitboxes = malloc(sizeof(Hitbox) * all->level.nb_hitboxes);
    if (all->level.hitboxes == NULL && all->level.nb_hitboxes != 0)
    {
        fprintf(stderr, "load_level : Out of Memory\n");
        return;
    }
    fread(all->level.hitboxes, sizeof(Hitbox), all->level.nb_hitboxes, file);

    fclose(file);

    move_cam_Level(-144, -112, &all->level);
}

void save_level(Everything *all)
{
    char file_path[200];
    FILE *file = NULL;
    int max_x, max_y;

    printf("Ecrire le chemin du fichier à sauvegarder : ");
    scanf(" %s", file_path);
    
    file = fopen(file_path, "wb");
    if (file == NULL)
    {
        fprintf(stderr, "save_level : impossible d'ouvrir ou de créer un fichier '%s'\n", file_path);
        return;
    }

    if (all->tile_array != NULL)
    {
        all->level.src.x = all->tile_array[0].dst.x;
        all->level.src.y = all->tile_array[0].dst.y;
    }
    for (int i = 0; i < all->nb_tiles; i++)
    {
        if (all->level.src.x > all->tile_array[i].dst.x)
        {
            all->level.src.x = all->tile_array[i].dst.x;
        }
        if (all->level.src.y > all->tile_array[i].dst.y)
        {
            all->level.src.y = all->tile_array[i].dst.y;
        }
    }
    all->level.src.w = 320;
    all->level.src.h = 240;
    all->level.size_x = 0;
    all->level.size_y = 0;
    for (int i = 0; i < all->nb_tiles; i++)
    {
        if (all->level.size_x < all->tile_array[i].dst.x - all->level.src.x)
        {
            all->level.size_x = all->tile_array[i].dst.x - all->level.src.x;
        }
        if (all->level.size_y < all->tile_array[i].dst.y - all->level.src.y)
        {
            all->level.size_y = all->tile_array[i].dst.y - all->level.src.y;
        }
    }
    all->level.size_x += 16;
    all->level.size_y += 16;
    fwrite(&all->level, sizeof(Level), 1, file);
    fwrite(&all->nb_tiles, sizeof(int), 1, file);
    fwrite(all->tile_array, sizeof(Tile), all->nb_tiles, file);
    fwrite(all->level.hitboxes, sizeof(Hitbox), all->level.nb_hitboxes, file);

    fclose(file);

    all->input.quit = SDL_TRUE;
}

void edit_level(Everything *all)
{
    SDL_bool destroy = SDL_FALSE;
    Tile new_tile;
    if (all->input.erase || all->input.draw)
    {
        for (int i = 0; i < all->nb_tiles; i++)
        {
            if (destroy)
            {
                all->tile_array[i-1] = all->tile_array[i];
            }
            if (all->tile_array[i].dst.x == all->cur_level.tile_src_dst.x && all->tile_array[i].dst.y == all->cur_level.tile_src_dst.y)
            {
                destroy = SDL_TRUE;
            }
        }
        if (destroy)
        {
            all->nb_tiles -= 1;
        }
    }
    if (all->input.draw)
    {
        all->nb_tiles += 1;
        all->tile_array = realloc(all->tile_array, sizeof(Tile) * all->nb_tiles);
        new_tile.tilemap_id = all->cur_tilemap_indice;
        new_tile.src = all->cur_tilemap.tile_src_dst;
        new_tile.dst = all->cur_level.tile_src_dst;
        all->tile_array[all->nb_tiles-1] = new_tile;
    }
}

void edit_hitbox(Everything *all)
{
    SDL_bool destroy = SDL_FALSE;
    if (all->input.erase)
    {
        for (int i = 0; i < all->level.nb_hitboxes; i++)
        {
            if (destroy)
            {
                all->level.hitboxes[i-1] = all->level.hitboxes[i];
            }
            if (all->cur_hitbox.tile_src_dst.x == all->level.hitboxes[i].points[0].x && all->cur_hitbox.tile_src_dst.y == all->level.hitboxes[i].points[0].y)
            {
                destroy = SDL_TRUE;
            }
        }
        if (destroy)
        {
            all->level.nb_hitboxes -= 1;
        }
    }
    if (all->input.draw)
    {
        all->hitbox_edit.points[all->cur_point_indice].x = all->cur_hitbox.tile_src_dst.x;
        all->hitbox_edit.points[all->cur_point_indice].y = all->cur_hitbox.tile_src_dst.y;
        all->cur_point_indice += 1;
        if (all->cur_point_indice == 4)
        {
            all->cur_point_indice = 0;
            all->level.nb_hitboxes += 1;
            all->level.hitboxes = realloc(all->level.hitboxes, sizeof(Hitbox) * all->level.nb_hitboxes);
            all->level.hitboxes[all->level.nb_hitboxes - 1] = all->hitbox_edit;
        }
        resetKeyState_Input(all->input.key_draw, &all->input);
    }
}

void load_Setup(Everything *all)
{
    loadKeys(&all->input);

    all->tilemap_array = load_Tilemaps(all->renderer);
    all->cur_tilemap_indice = 0;
    all->nb_tiles = 0;
    all->tile_array = NULL;

    all->cur_point_indice = 0;
    all->level.nb_hitboxes = 0;
    all->level.hitboxes = NULL;
    all->level.size_x = 0;
    all->level.size_y = 0;
    all->level.texture = NULL;
    all->level.src.h = 240;
    all->level.src.w = 320;
    all->level.src.x = 0;
    all->level.src.y = 0;
    
    all->cur_tilemap.dst.h = 16;
    all->cur_tilemap.dst.w = 16;
    all->cur_tilemap.dst.x = 0;
    all->cur_tilemap.dst.y = 0;
    all->cur_level.dst = all->cur_tilemap.dst;
    all->cur_level.dst.x = 320 + 144;
    all->cur_level.dst.y = 112;
    all->cur_hitbox.dst = all->cur_level.dst;
    all->cur_tilemap.tile_src_dst = all->cur_tilemap.dst;
    all->cur_level.tile_src_dst = all->cur_level.dst;
    all->cur_level.tile_src_dst.x -= 320;
    all->cur_hitbox.tile_src_dst = all->cur_level.tile_src_dst;

    all->cur_tilemap.texture_active = loadImage("level_editor/cursor.bmp", all->renderer);
    all->cur_tilemap.texture_inactive = loadImage("level_editor/cursor_disabled.bmp", all->renderer);
    all->cur_level.texture_active = loadImage("level_editor/cursor.bmp", all->renderer);
    all->cur_level.texture_inactive = loadImage("level_editor/cursor_disabled.bmp", all->renderer);
    all->cur_hitbox.texture_active = loadImage("level_editor/cursor_hitbox.bmp", all->renderer);
    all->cur_hitbox.texture_inactive = NULL;

    load_level(all);
    all->mode = TILEMAP;
}

void run_tilemapFrame(Everything *all)
{
    if (all->input.down && all->cur_tilemap.dst.y < 480 - 16)
    {
        all->cur_tilemap.dst.y += 16;
        all->cur_tilemap.tile_src_dst.y += 16;
        resetKeyState_Input(all->input.key_down, &all->input);
    }
    if (all->input.up && all->cur_tilemap.dst.y > 0)
    {
        all->cur_tilemap.dst.y -= 16;
        all->cur_tilemap.tile_src_dst.y -= 16;
        resetKeyState_Input(all->input.key_up, &all->input);
    }
    if (all->input.right && all->cur_tilemap.dst.x < 320 - 16)
    {
        all->cur_tilemap.dst.x += 16;
        all->cur_tilemap.tile_src_dst.x += 16;
        resetKeyState_Input(all->input.key_right, &all->input);
    }
    if (all->input.left && all->cur_tilemap.dst.x > 0)
    {
        all->cur_tilemap.dst.x -= 16;
        all->cur_tilemap.tile_src_dst.x -= 16;
        resetKeyState_Input(all->input.key_left, &all->input);
    }

    if (all->input.tilemap_add)
    {
        if (all->cur_tilemap_indice < NB_TILEMAPS-1)
        {
            all->cur_tilemap_indice += 1;
        }
        else
        {
            all->cur_tilemap_indice = 0;
        }
        resetKeyState_Input(all->input.key_tilemap_add, &all->input);
    }
    if (all->input.tilemap_sub)
    {
        if (all->cur_tilemap_indice > 0)
        {
            all->cur_tilemap_indice -= 1;
        }
        else
        {
            all->cur_tilemap_indice = NB_TILEMAPS-1;
        }
        resetKeyState_Input(all->input.key_tilemap_sub, &all->input);
    }
    if (all->input.switch_cursor)
    {
        all->mode = LEVEL;
        resetKeyState_Input(all->input.key_switch_cursor, &all->input);
    }
    if (all->input.toggle_hitbox)
    {
        all->mode = HITBOX;
        all->cur_hitbox.tile_src_dst = all->cur_level.tile_src_dst;
        resetKeyState_Input(all->input.key_toggle_hitbox, &all->input);
    }

    SDL_Rect dst_tilemap = {.h = 480, .w = 320, .x = 0, .y = 0};
    if (all->tilemap_array != NULL)
    {
        SDL_RenderCopy(all->renderer, all->tilemap_array[all->cur_tilemap_indice], NULL, &dst_tilemap);
    }
    display_LevelEdit(all);
    if (all->cur_tilemap.texture_active != NULL)
    {
        SDL_RenderCopy(all->renderer, all->cur_tilemap.texture_active, NULL, &all->cur_tilemap.dst);
    }
    if (all->cur_level.texture_inactive != NULL)
    {
        SDL_RenderCopy(all->renderer, all->cur_level.texture_inactive, NULL, &all->cur_level.dst);
    }

    if (all->input.save)
    {
        save_level(all);
    }
}

void run_levelFrame(Everything *all)
{
    if (all->input.down)
    {
        move_cam_Level(0, 16, &all->level);
        all->cur_level.tile_src_dst.y += 16;
        resetKeyState_Input(all->input.key_down, &all->input);
    }
    if (all->input.up)
    {
        move_cam_Level(0, -16, &all->level);
        all->cur_level.tile_src_dst.y -= 16;
        resetKeyState_Input(all->input.key_up, &all->input);
    }
    if (all->input.right)
    {
        move_cam_Level(16, 0, &all->level);
        all->cur_level.tile_src_dst.x += 16;
        resetKeyState_Input(all->input.key_right, &all->input);
    }
    if (all->input.left)
    {
        move_cam_Level(-16, 0, &all->level);
        all->cur_level.tile_src_dst.x -= 16;
        resetKeyState_Input(all->input.key_left, &all->input);
    }

    edit_level(all);
    if (all->input.switch_cursor)
    {
        all->mode = TILEMAP;
        resetKeyState_Input(all->input.key_switch_cursor, &all->input);
    }
    if (all->input.toggle_hitbox)
    {
        all->mode = HITBOX;
        all->cur_hitbox.tile_src_dst = all->cur_level.tile_src_dst;
        resetKeyState_Input(all->input.key_toggle_hitbox, &all->input);
    }

    SDL_Rect dst_tilemap = {.h = 480, .w = 320, .x = 0, .y = 0};
    if (all->tilemap_array != NULL)
    {
        SDL_RenderCopy(all->renderer, all->tilemap_array[all->cur_tilemap_indice], NULL, &dst_tilemap);
    }
    display_LevelEdit(all);
    if (all->cur_tilemap.texture_inactive != NULL)
    {
        SDL_RenderCopy(all->renderer, all->cur_tilemap.texture_inactive, NULL, &all->cur_tilemap.dst);
    }
    if (all->cur_level.texture_active != NULL)
    {
        SDL_RenderCopy(all->renderer, all->cur_level.texture_active, NULL, &all->cur_level.dst);
    }

    if (all->input.save)
    {
        save_level(all);
    }
}

void run_hitboxFrame(Everything *all)
{
    if (all->input.down)
    {
        move_cam_Level(0, 16, &all->level);
        all->cur_hitbox.tile_src_dst.y += 16;
        resetKeyState_Input(all->input.key_down, &all->input);
    }
    if (all->input.up)
    {
        move_cam_Level(0, -16, &all->level);
        all->cur_hitbox.tile_src_dst.y -= 16;
        resetKeyState_Input(all->input.key_up, &all->input);
    }
    if (all->input.right)
    {
        move_cam_Level(16, 0, &all->level);
        all->cur_hitbox.tile_src_dst.x += 16;
        resetKeyState_Input(all->input.key_right, &all->input);
    }
    if (all->input.left)
    {
        move_cam_Level(-16, 0, &all->level);
        all->cur_hitbox.tile_src_dst.x -= 16;
        resetKeyState_Input(all->input.key_left, &all->input);
    }

    edit_hitbox(all);
    if (all->input.toggle_hitbox)
    {
        all->mode = LEVEL;
        all->cur_level.tile_src_dst = all->cur_hitbox.tile_src_dst;
        resetKeyState_Input(all->input.key_toggle_hitbox, &all->input);
    }

    display_LevelEdit(all);
    for (int i = 0; i < all->level.nb_hitboxes; i++)
    {
        display_Hitbox(&all->level.hitboxes[i], 320 - all->level.src.x, - all->level.src.y, all->renderer);
    }
    SDL_SetRenderDrawColor(all->renderer, 255, 0, 0, 255);
    for (int i = 0; i < all->cur_point_indice-1; i++)
    {
        SDL_RenderDrawLine(all->renderer, all->hitbox_edit.points[i].x - all->level.src.x + 320, all->hitbox_edit.points[i].y - all->level.src.y,
                            all->hitbox_edit.points[i+1].x - all->level.src.x + 320, all->hitbox_edit.points[i+1].y - all->level.src.y);
    }
    SDL_SetRenderDrawColor(all->renderer, 0, 0, 0, 255);
    if (all->cur_hitbox.texture_active != NULL)
    {
        SDL_RenderCopy(all->renderer, all->cur_hitbox.texture_active, NULL, &all->cur_hitbox.dst);
    }

    if (all->input.save)
    {
        save_level(all);
    }
}

void runFrame(Everything *all)
{
    SDL_RenderClear(all->renderer);
    switch (all->mode)
    {
    case SETUP :
        load_Setup(all);
        break;
    case TILEMAP :
        run_tilemapFrame(all);
        break;
    case LEVEL :
        run_levelFrame(all);
        break;
    case HITBOX :
        run_hitboxFrame(all);
        break;
    };
    SDL_RenderPresent(all->renderer);
}

int main(int argc, char *argv[])
{
    /* Création des variables */

    Everything all = {.renderer = NULL, .window = NULL, .mode = SETUP, .input.quit = SDL_FALSE, .cur_tilemap_indice = 0};
    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
        all.input.key[i] = SDL_FALSE;

    /* Initialisations, création de la fenêtre et du renderer. */

    init(&all, "Level Editor", "none", 2*320, 2*240, SDL_FALSE);

    /* Boucle principale du jeu */

    while (!all.input.quit)
    {
        update_Input(&all.input);
        runFrame(&all);
        SDL_Delay((int)(1000 / 60));
    }

    /* Fermeture du logiciel et libération de la mémoire */

    quit(&all, EXIT_SUCCESS);
}