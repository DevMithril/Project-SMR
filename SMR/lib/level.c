#include "level.h"

SDL_Texture **load_Tilemaps(SDL_Renderer *renderer)
{
    SDL_Texture **tilemaps = malloc(sizeof(SDL_Texture*) * NB_TILEMAPS);
    if (tilemaps == NULL && NB_TILEMAPS != 0)
    {
        fprintf(stderr, "load_Tilemaps : Out of Memory\n");
        return NULL;
    }
    tilemaps[0] = loadImage("assets/tilemaps/grass.bmp", renderer);
    tilemaps[1] = loadImage("assets/tilemaps/MF_AQA.bmp", renderer);
    tilemaps[2] = loadImage("assets/tilemaps/MF_ARC.bmp", renderer);
    tilemaps[3] = loadImage("assets/tilemaps/MF_COM.bmp", renderer);
    tilemaps[4] = loadImage("assets/tilemaps/MF_MAIN.bmp", renderer);
    tilemaps[5] = loadImage("assets/tilemaps/MF_NOC.bmp", renderer);
    tilemaps[6] = loadImage("assets/tilemaps/MF_PYR.bmp", renderer);
    tilemaps[7] = loadImage("assets/tilemaps/MF_SRX.bmp", renderer);
    tilemaps[8] = loadImage("assets/tilemaps/MF_TRO.bmp", renderer);
    tilemaps[9] = loadImage("assets/tilemaps/MZM_brinstar.bmp", renderer);
    tilemaps[10] = loadImage("assets/tilemaps/MZM_chozodia.bmp", renderer);
    tilemaps[11] = loadImage("assets/tilemaps/MZM_crateria.bmp", renderer);
    tilemaps[12] = loadImage("assets/tilemaps/MZM_kraid.bmp", renderer);
    tilemaps[13] = loadImage("assets/tilemaps/MZM_norfair.bmp", renderer);
    tilemaps[14] = loadImage("assets/tilemaps/MZM_ridley.bmp", renderer);
    tilemaps[15] = loadImage("assets/tilemaps/MZM_tourian.bmp", renderer);
    return tilemaps;
}

void destroy_Tilemaps(SDL_Texture **tilemaps)
{
    for (int i = 0; i < NB_TILEMAPS; i++)
    {
        SDL_DestroyTexture(tilemaps[i]);
    }
    free(tilemaps);
}

void load_Level(int level_id, Level *level, SDL_Renderer *renderer)
{
    FILE *file = fopen("assets/levels/platform.dat", "rb");
    int nb_tile;
    Tile *tiles;
    SDL_Texture **tilemaps;
    fread(level, sizeof(Level), 1, file);
    
    // lecture des tiles et construction de la texture

    fread(&nb_tile, sizeof(int), 1, file);
    tiles = malloc(sizeof(Tile) * nb_tile);
    if (tiles == NULL && nb_tile != 0)
    {
        fprintf(stderr, "load_Level : Out of Memory\n");
        return;
    }
    fread(tiles, sizeof(Tile), nb_tile, file);
    level->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, level->size_x, level->size_y);
    if(NULL == level->texture)
    {
        fprintf(stderr, "Erreur SDL_CreateTexture : %s\n", SDL_GetError());
        free(tiles);
        return;
    }
    SDL_SetTextureBlendMode(level->texture, SDL_BLENDMODE_BLEND);
    tilemaps = load_Tilemaps(renderer);
    SDL_SetRenderTarget(renderer, level->texture);
    for (int i = 0; i < nb_tile; i++)
    {
        SDL_RenderCopy(renderer, tilemaps[tiles[i].tilemap_id], &tiles[i].src, &tiles[i].dst);
    }
    destroy_Tilemaps(tilemaps);
    SDL_SetRenderTarget(renderer, NULL);
    free(tiles);

    // lecture des hitboxes

    level->hitboxes = NULL;
    level->hitboxes = malloc(sizeof(Hitbox) * level->nb_hitboxes);
    if (level->hitboxes == NULL && level->nb_hitboxes != 0)
    {
        fprintf(stderr, "load_Level : Out of Memory\n");
        return;
    }
    fread(level->hitboxes, sizeof(Hitbox), level->nb_hitboxes, file);
    
    level->cam_hitboxes = NULL;
    level->cam_hitboxes = malloc(sizeof(Hitbox) * level->cam_nb_hitboxes);
    if (level->cam_hitboxes == NULL && level->cam_nb_hitboxes != 0)
    {
        fprintf(stderr, "load_Level : Out of Memory\n");
        return;
    }
    fread(level->cam_hitboxes, sizeof(Hitbox), level->cam_nb_hitboxes, file);

    fclose(file);
}

void destroy_Level(Level *level)
{
    if (level->texture != NULL)
    {
        SDL_DestroyTexture(level->texture);
    }
    if (level->hitboxes != NULL)
    {
        free(level->hitboxes);
    }
    if (level->cam_hitboxes != NULL)
    {
        free(level->cam_hitboxes);
    }
}

Vector2 move_cam_Level(int x, int y, Level *level)
{
    Vector2 mtv;
    Hitbox cam;
    level->src.x += x;
    level->src.y += y;
    cam.points[0].x = level->src.x;
    cam.points[0].y = level->src.y;
    cam.points[1].x = level->src.x + level->src.w;
    cam.points[1].y = level->src.y;
    cam.points[2].x = level->src.x + level->src.w;
    cam.points[2].y = level->src.y + level->src.h;
    cam.points[3].x = level->src.x;
    cam.points[3].y = level->src.y + level->src.h;
    mtv = sat_bulk(&cam, level->cam_hitboxes, level->cam_nb_hitboxes);
    level->src.x += mtv.x;
    level->src.y += mtv.y;
    mtv.x = -mtv.x;
    mtv.y = -mtv.y;
    return mtv;
}

void display_Level(Level *level, SDL_Renderer *renderer)
{
    render_Texture(renderer, level->texture, &level->src, NULL);
}