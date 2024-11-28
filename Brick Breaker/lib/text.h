#ifndef TEXT__LIB__H
#define TEXT__LIB__H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Text
{
    SDL_Texture *texture;
    SDL_Rect src_rect, dst_rect;
}Text;

/* permet d'allouer une structure Text, renvoie NULL en cas d'erreur */
Text *create_Text(TTF_Font *font, const char text[], SDL_Color color, SDL_Renderer *renderer);

/* permet d'écrire les données d'un text dans un fichier */
void save_Text(FILE **file, const char *text_file, int num_paragraph, const char *font_file, int font_size, SDL_Color color);

/* permet d'allouer un text à partir de données présentes dans un fichier ; renvoie NULL en cas d'erreur */
Text *load_Text(FILE **file, SDL_Renderer *renderer);

/* permet d'allouer un text qui contient un texte d'un nombre quelconque de lignes issues d'un fichier texte */
Text *read_Text(const char *text_file_path, int num_paragraph, const char *font_file_path, int font_size, SDL_Color color, SDL_Renderer *renderer);

/* permet de libèrer la mémoire allouée pour un text */
void destroy_Text(Text **text);

/* permet d'afficher un text */
void display_Text(Text *text, SDL_Renderer *renderer);

/* permet de déplacer un text */
void move_Text(int x, int y, Text *text);

/* permet de créer un text à partir d'une saisie */
Text *scan_Text(char *text, const char *font_file_path, int font_size, SDL_Color color, SDL_Renderer *renderer, SDL_bool *input_quit);

#endif