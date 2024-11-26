#include "text.h"

Text *create_Text(TTF_Font *font, const char text[], SDL_Color color, SDL_Renderer *renderer)
{
    SDL_Surface *surface = NULL; 
    SDL_Texture *texture = NULL, *tmp = NULL;
    Text *new = NULL;
    surface = TTF_RenderText_Solid(font, text, color);
    if(NULL == surface)
    {
        fprintf(stderr, "Erreur TTF_RenderText_Solid : %s\n", TTF_GetError());
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
    new = SDL_malloc(sizeof(Text));
    if (NULL == new)
    {
        fprintf(stderr, "Erreur SDL_CreateTexture : %s\n", SDL_GetError());
        return NULL;
    }
    new->texture = texture;
    new->src_rect.h = surface->h;
    new->src_rect.w = surface->w;
    new->src_rect.x = 0;
    new->src_rect.y = 0;
    new->dst_rect.h = surface->h;
    new->dst_rect.w = surface->w;
    new->dst_rect.x = 0;
    new->dst_rect.y = 0;

    SDL_SetRenderTarget(renderer, texture);
    SDL_RenderCopy(renderer, tmp, NULL, NULL);
    SDL_DestroyTexture(tmp);
    SDL_FreeSurface(surface);
    SDL_SetRenderTarget(renderer, NULL);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return new;
}

void save_Text(FILE **file, const char *text, const char *font_file, int font_size, int a, int r, int g, int b)
{
    char t[LEN_MAX_TEXT];
    char font_f[MAX_LEN_FILE_NAME];
    SDL_Color color = {.a = a, .r = r, .g = g, .b = b};
    fwrite(text, sizeof(t), 1, *file);
    fwrite(font_file, sizeof(font_f), 1, *file);
    fwrite(&font_size, sizeof(int), 1, *file);
    fwrite(&color, sizeof(SDL_Color), 1, *file);
}

Text *load_Text(FILE **file, Text *text, SDL_Renderer *renderer)
{
    char text[LEN_MAX_TEXT];
    char font_file[MAX_LEN_FILE_NAME];
    int font_size;
    TTF_Font *font;
    SDL_Color color;
    Text *new = NULL;
    fread(text, sizeof(text), 1, *file);
    fread(font_file, sizeof(font_file), 1, *file);
    fread(&font_size, sizeof(int), 1, *file);
    fread(&color, sizeof(SDL_Color), 1, *file);
    font = TTF_OpenFont(font_file, font_size);
    new = create_Text(font, text, color, renderer);
    TTF_CloseFont(font);
    return new;
}

void destroy_Text(Text **text)
{
    if (*text == NULL)
    {
        return;
    }
    if ((*text)->texture != NULL)
    {
        SDL_DestroyTexture((*text)->texture);
    }
    SDL_free(*text);
    *text = NULL;
}

void display_Text(Text *text, SDL_Renderer *renderer)
{
    if (text->texture != NULL)
    {
        SDL_RenderCopy(renderer, text->texture, &text->src_rect, &text->dst_rect);
    }
}

void move_Text(int x, int y, Text *text)
{
    text->dst_rect.x += x;
    text->dst_rect.y += y;
}