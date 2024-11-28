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
        SDL_FreeSurface(surface);
        return NULL;
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, 
                            SDL_TEXTUREACCESS_TARGET, surface->w, surface->h);
    if(NULL == texture)
    {
        fprintf(stderr, "Erreur SDL_CreateTexture : %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(tmp);
        return NULL;
    }
    new = SDL_malloc(sizeof(Text));
    if (NULL == new)
    {
        fprintf(stderr, "Erreur SDL_malloc : %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(tmp);
        SDL_DestroyTexture(texture);
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

void save_Text(FILE **file, const char *text_file, int num_paragraph, const char *font_file, int font_size, SDL_Color color)
{
    size_t size_text_file = sizeof(text_file);
    size_t size_font_file = sizeof(font_file);
    fwrite(&size_text_file, sizeof(size_t), 1, *file);
    fwrite(text_file, size_text_file, 1, *file);
    fwrite(&size_font_file, sizeof(size_t), 1, *file);
    fwrite(font_file, size_font_file, 1, *file);
    fwrite(&font_size, sizeof(int), 1, *file);
    fwrite(&color, sizeof(SDL_Color), 1, *file);
    fwrite(&num_paragraph, sizeof(int), 1, *file);
}

Text *load_Text(FILE **file, SDL_Renderer *renderer)
{
    size_t size_text_file;
    size_t size_font_file;
    int font_size;
    SDL_Color color;
    int num_paragraph;
    Text *new = NULL;
    fread(&size_text_file, sizeof(size_t), 1, *file);
    char text_file[size_text_file];
    fread(text_file, size_text_file, 1, *file);
    fread(&size_font_file, sizeof(size_t), 1, *file);
    char font_file[size_font_file];
    fread(font_file, size_font_file, 1, *file);
    fread(&font_size, sizeof(int), 1, *file);
    fread(&color, sizeof(SDL_Color), 1, *file);
    fread(&num_paragraph, sizeof(int), 1, *file);
    new = read_Text(text_file, num_paragraph, font_file, font_size, color, renderer);
    return new;
}

Text *add_line(Text *origin, const char *line, TTF_Font *font, SDL_Color color, SDL_Renderer *renderer)
{
    Text *new = NULL;
    Text *new_line = NULL;
    new_line = create_Text(font, line, color, renderer);
    int o_h, o_w, nl_h, nl_w;
    if (origin == NULL)
    {
        if(!(strcmp(line, "&~")))
        {
            SDL_QueryTexture(new_line->texture, NULL, NULL, &nl_w, &nl_h);
            SDL_DestroyTexture(new_line->texture);
            new_line->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, nl_w, nl_h);
        }
        return new_line;
    }
    if (new_line == NULL)
    {
        return origin;
    }
    new = SDL_malloc(sizeof(Text));
    if (NULL == new)
    {
        fprintf(stderr, "Erreur SDL_malloc : %s\n", SDL_GetError());
        return NULL;
    }
    SDL_QueryTexture(origin->texture, NULL, NULL, &o_w, &o_h);
    SDL_QueryTexture(new_line->texture, NULL, NULL, &nl_w, &nl_h);
    int max_w;
    if (o_w > nl_w)
    {
        max_w = o_w;
    }
    else
    {
        max_w = nl_w;
    }
    new->src_rect.h = o_h + nl_h;
    new->src_rect.w = max_w;
    new->src_rect.x = 0;
    new->src_rect.y = 0;
    new->dst_rect.h = o_h + nl_h;
    new->dst_rect.w = max_w;
    new->dst_rect.x = 0;
    new->dst_rect.y = 0;
    new->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, max_w, o_h + nl_h);
    if (new->texture == NULL)
    {
        fprintf(stderr, "Erreur SDL_CreateTexture : %s\n", SDL_GetError());
        SDL_free(new);
        destroy_Text(&new_line);
        return origin;
    }
    new_line->dst_rect.y = origin->dst_rect.h;
    SDL_SetRenderTarget(renderer, new->texture);
    SDL_RenderCopy(renderer, origin->texture, NULL, &origin->dst_rect);
    destroy_Text(&origin);
    if(strcmp(line, "&~"))
    {
        SDL_RenderCopy(renderer, new_line->texture, NULL, &new_line->dst_rect);
    }
    destroy_Text(&new_line);
    SDL_SetRenderTarget(renderer, NULL);
    SDL_SetTextureBlendMode(new->texture, SDL_BLENDMODE_BLEND);
    return new;
}

Text *read_Text(const char *text_file_path, int num_paragraph, const char *font_file_path, int font_size, SDL_Color color, SDL_Renderer *renderer)
{
    FILE *text_file = NULL;
    char line[200];
    TTF_Font *font = NULL;
    Text *new = NULL;
    font = TTF_OpenFont(font_file_path, font_size);
    if (font == NULL)
    {
        fprintf(stderr, "Erreur dans read_Text : impossible d'ouvrir '%s'\n", font_file_path);
        return NULL;
    }
    text_file = fopen(text_file_path, "r");
    if (text_file == NULL)
    {
        fprintf(stderr, "Erreur dans read_Text : impossible d'ouvrir '%s'\n", text_file_path);
        TTF_CloseFont(font);
        return NULL;
    }
    for (int i = 0; i < num_paragraph; i++)
    {
        do
        {
            fscanf(text_file, " %[^\n]", line);
        } while (strcmp(line, "#$"));
    }
    fscanf(text_file, "\n%[^\n]", line);
    while (strcmp(line, "#$"))
    {
        new = add_line(new, line, font, color, renderer);
        fscanf(text_file, "\n%[^\n]", line);
    }
    fclose(text_file);
    TTF_CloseFont(font);
    return new;
}

void destroy_Text(Text **text)
{
    if ((*text) == NULL)
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
    if (text == NULL)
    {
        return;
    }
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

Text *scan_Text(char *text, const char *font_file_path, int font_size, SDL_Color color, SDL_Renderer *renderer, SDL_bool *input_quit)
{
    Text *new = NULL;
    TTF_Font *font = TTF_OpenFont(font_file_path, font_size);
    SDL_bool done = SDL_FALSE;
    strcpy(text, "");
    SDL_StartTextInput();
    while (!done)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event) && !done)
        {
            if(event.type == SDL_QUIT)
            {
                *input_quit = SDL_TRUE;
                done = SDL_TRUE;
            }
            else if (event.type == SDL_TEXTINPUT)
            {
                strcat(text, event.text.text);
            }
            else if(event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.scancode == SDL_SCANCODE_RETURN)
                {
                    done = SDL_TRUE;
                }
            }
        }
        SDL_Delay((int)(1000/60));
    }
    SDL_StopTextInput();
    new = create_Text(font, text, color, renderer);
    TTF_CloseFont(font);
    return new;
}