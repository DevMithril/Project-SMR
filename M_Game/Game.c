#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

/* resolution : 480x270 */
/* ligne de commande pour la compilation : gcc -o M_Game Game.c $(sdl2-config --cflags --libs) */
/* pour utiliser valgrind (memoire) : valgrind -s --tool=memcheck --leak-check=yes|no|full|summary --leak-resolution=low|med|high --show-reachable=yes ./M_Game */

typedef struct Vecteur
{
    int x;
    int y;
}Vecteur;

typedef struct Hitbox
{
    SDL_Point A;
    SDL_Point B;
}Hitbox;

typedef struct Entity
{
    int Id, type;
    char info[41];
    int info_1, info_2, info_3, info_4;
    SDL_Texture *texture;
    int nb_sprites, hauteur_sprite, largeur_sprite;
    SDL_Rect src_render_texture, target_render_texture;
    int nb_segments;
    Hitbox *hitbox;
    int x, y;
    SDL_bool affected_by_gravity, can_jump, jumping, on_ground, jump_command_down, right_command_down, left_command_down;
    int jump_high, jump_high_max, jump_speed, fall_high, fall_speed;
    int left_dist, right_dist, speed, speed_max;
    struct Entity *suivant;
}Entity;

typedef struct Game
{
    int game_state;
}Game;

void load_Entity_infos(Entity *entity)
{
    switch (entity->Id)
    {
        case 0 :
            break;
        case 1 :
            break;
        default:
            break;
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

Entity *load_Entity(const char chemin_file[], SDL_Renderer *renderer, Entity *liste)
{
    FILE *fichier = NULL;
    Entity *new_entity = NULL;
    new_entity = malloc(sizeof(Entity));
    if (new_entity == NULL)
        return new_entity;
    
    int origine_rect_x = 0;
    int origine_rect_y = 0;
    char chemin_image[41];
    int i;

    fichier = fopen(chemin_file, "r");
    if (fichier != NULL)
    {
        fscanf(fichier, "%d %s %d %d %d ", &new_entity->Id, chemin_image, &new_entity->nb_sprites, &new_entity->hauteur_sprite, &new_entity->largeur_sprite);
        fscanf(fichier, "%d %d / %d // %d :", &origine_rect_x, &origine_rect_y, &new_entity->type, &new_entity->nb_segments);
        Hitbox hitbox[new_entity->nb_segments];
        new_entity->hitbox = malloc(sizeof(hitbox));
        for (i = 0; i < new_entity->nb_segments; i++)
        {
            fscanf(fichier, " %d %d %d %d /", &new_entity->hitbox[i].A.x, &new_entity->hitbox[i].A.y, &new_entity->hitbox[i].B.x, &new_entity->hitbox[i].B.y);
        }
        fclose(fichier);
    }
    else
        printf("L'ouverture du fichier %s a échouée\n", chemin_file);
    
    if (strcmp(chemin_image,"none") != 0)
        new_entity->texture = loadImage(chemin_image, renderer);
    else
        new_entity->texture == NULL;
    
    new_entity->x = 0;
    new_entity->y = 0;
    new_entity->target_render_texture.x = origine_rect_x;
    new_entity->target_render_texture.y = origine_rect_y;
    new_entity->target_render_texture.w = new_entity->largeur_sprite;
    new_entity->target_render_texture.h = new_entity->hauteur_sprite;
    new_entity->src_render_texture.x = 0;
    new_entity->src_render_texture.y = 0;
    new_entity->src_render_texture.w = new_entity->largeur_sprite;
    new_entity->src_render_texture.h = new_entity->hauteur_sprite;

    load_Entity_infos(new_entity);

    new_entity->suivant = NULL;
    while (NULL != liste->suivant)
        liste = liste->suivant;
    liste->suivant = new_entity;

    return new_entity;
}

void destroy_Entity(Entity *entity, Entity *liste)
{
    if (entity == NULL)
    {
        printf("Erreur dans destroy_Entity : l'entité passée en paramètre est un pointeur null \n");
        return;
    }
    while (entity != liste->suivant)
    {
        liste = liste->suivant;
    }

    Entity *next = liste->suivant->suivant;
    if (liste->suivant->texture != NULL)
    {
        SDL_DestroyTexture(liste->suivant->texture);
        liste->suivant->texture = NULL;
    }
    free(liste->suivant->hitbox);
    free(liste->suivant);
    liste->suivant = next;
}

int Quit(SDL_Window *window, SDL_Renderer *renderer, SDL_Surface *icone, Entity *liste, int statut)
{
    while (NULL != liste->suivant)
    {
        destroy_Entity(liste->suivant, liste);
    }
    if (NULL == liste->suivant)
    {
        if (liste->type == 0)
            printf("Libération des Entités effectuée\n");
    }
    if (NULL != icone)
        SDL_FreeSurface(icone);
    if (NULL != renderer)
        SDL_DestroyRenderer(renderer);
    if (NULL != window)
        SDL_DestroyWindow(window);
    SDL_Quit();
    printf("Libérations de toutes les ressources réussies\n");
    return statut;
}

void Init(SDL_Window **window, SDL_Renderer **renderer, SDL_Surface **icone, Entity *liste, int statut)
{
    if (0 != SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "Erreur SDL_Init : %s", SDL_GetError());
        Quit(*window, *renderer, *icone, liste, statut);
    }
    *window = SDL_CreateWindow("M Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              10*480, 10*270, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
    if (NULL == *window)
    {
        fprintf(stderr, "Erreur SDL_CreateWindow : %s", SDL_GetError());
        Quit(*window, *renderer, *icone, liste, statut);
    }
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (NULL == *renderer)
    {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s", SDL_GetError());
        Quit(*window, *renderer, *icone, liste, statut);
    }
    SDL_RenderSetLogicalSize(*renderer, 480, 270);
    *icone = SDL_LoadBMP("icone.bmp");
    if (NULL == *icone)
    {
        fprintf(stderr, "Erreur SDL_LoadBMP : %s", SDL_GetError());
        Quit(*window, *renderer, *icone, liste, statut);
    }
    SDL_SetWindowIcon(*window, *icone);
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

SDL_bool sat(Entity *entity1, Entity *entity2)
{
    int i = 0, j = 0;
    double min1 = 0, max1 = 0, min2 = 0, max2 = 0, p = 0;
    Vecteur normal = {.x = 0, .y = 0};


    for (i = 0; i < (entity1->nb_segments)-1; i++ )
    {
        normal.x = -(entity1->hitbox[i].A.y - entity1->hitbox[i].B.y);
        normal.y = entity1->hitbox[i].A.x - entity1->hitbox[i].B.x;
        min1 = normal.x * entity1->hitbox[0].A.x + normal.y * entity1->hitbox[0].A.y;
        max1 = min1;
        for (j = 1; j < (entity1->nb_segments)-1; j++ )
        {
            p = normal.x * entity1->hitbox[j].A.x + normal.y * entity1->hitbox[j].A.y;
            if (p < min1)
            {
                min1 = p;
            }
            else if (p > max1)
            {
                max1 = p;
            }
        }
        min2 = normal.x * entity2->hitbox[0].A.x + normal.y * entity2->hitbox[0].A.y;
        max2 = min2;
        for (j = 1; j < (entity2->nb_segments)-1; j++ )
        {
            p = normal.x * entity2->hitbox[j].A.x + normal.y * entity2->hitbox[j].A.y;
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
    

    for (i = 0; i < (entity2->nb_segments)-1; i++ )
    {
        normal.x = -(entity2->hitbox[i].A.y - entity2->hitbox[i].B.y);
        normal.y = entity2->hitbox[i].A.x - entity2->hitbox[i].B.x;
        min1 = normal.x * entity1->hitbox[0].A.x + normal.y * entity1->hitbox[0].A.y;
        max1 = min1;
        for (j = 1; j < (entity1->nb_segments)-1; j++ )
        {
            p = normal.x * entity1->hitbox[j].A.x + normal.y * entity1->hitbox[j].A.y;
            if (p < min1)
            {
                min1 = p;
            }
            else if (p > max1)
            {
                max1 = p;
            }
        }
        min2 = normal.x * entity2->hitbox[0].A.x + normal.y * entity2->hitbox[0].A.y;
        max2 = min2;
        for (j = 1; j < (entity2->nb_segments)-1; j++ )
        {
            p = normal.x * entity2->hitbox[j].A.x + normal.y * entity2->hitbox[j].A.y;
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

SDL_bool sat_seg(Entity *entity, Hitbox segment)
{
    int i = 0, j = 0;
    double min1 = 0, max1 = 0, min2 = 0, max2 = 0, p = 0;
    Vecteur normal = {.x = 0, .y = 0};


    for (i = 0; i < (entity->nb_segments)-1; i++ )
    {
        normal.x = -(entity->hitbox[i].A.y - entity->hitbox[i].B.y);
        normal.y = entity->hitbox[i].A.x - entity->hitbox[i].B.x;
        min1 = normal.x * entity->hitbox[0].A.x + normal.y * entity->hitbox[0].A.y;
        max1 = min1;
        for (j = 1; j < (entity->nb_segments)-1; j++ )
        {
            p = normal.x * entity->hitbox[j].A.x + normal.y * entity->hitbox[j].A.y;
            if (p < min1)
            {
                min1 = p;
            }
            else if (p > max1)
            {
                max1 = p;
            }
        }
        min2 = normal.x * segment.A.x + normal.y * segment.A.y;
        max2 = min2;
        p = normal.x * segment.B.x + normal.y * segment.B.y;
        if (p < min2)
        {
            min2 = p;
        }
        else if (p > max2)
        {
            max2 = p;
        }
        if (min1 > max2 || min2 > max1)
        {
            return SDL_FALSE;
        }
    }


    normal.x = -(segment.A.y - segment.B.y);
    normal.y = segment.A.x - segment.B.x;
    min1 = normal.x * entity->hitbox[0].A.x + normal.y * entity->hitbox[0].A.y;
    max1 = min1;
    for (j = 1; j < (entity->nb_segments)-1; j++ )
    {
        p = normal.x * entity->hitbox[j].A.x + normal.y * entity->hitbox[j].A.y;
        if (p < min1)
        {
            min1 = p;
        }
        else if (p > max1)
        {
            max1 = p;
        }
    }
    min2 = normal.x * segment.A.x + normal.y * segment.A.y;
    max2 = min2;
    p = normal.x * segment.B.x + normal.y * segment.B.y;
    if (p < min2)
    {
        min2 = p;
    }
    else if (p > max2)
    {
        max2 = p;
    }
    if (min1 > max2 || min2 > max1)
    {
        return SDL_FALSE;
    }


    normal.x = segment.A.x - segment.B.x;
    normal.y = segment.A.y - segment.B.y;
    min1 = normal.x * entity->hitbox[0].A.x + normal.y * entity->hitbox[0].A.y;
    max1 = min1;
    for (j = 1; j < (entity->nb_segments)-1; j++ )
    {
        p = normal.x * entity->hitbox[j].A.x + normal.y * entity->hitbox[j].A.y;
        if (p < min1)
        {
            min1 = p;
        }
        else if (p > max1)
        {
            max1 = p;
        }
    }
    min2 = normal.x * segment.A.x + normal.y * segment.A.y;
    max2 = min2;
    p = normal.x * segment.B.x + normal.y * segment.B.y;
    if (p < min2)
    {
        min2 = p;
    }
    else if (p > max2)
    {
        max2 = p;
    }
    if (min1 > max2 || min2 > max1)
    {
        return SDL_FALSE;
    }
    return SDL_TRUE;
}

SDL_bool collision(Entity *entity, int type, Entity *liste)
{
    int i = 0;
    liste = liste->suivant;
    if (type = 2)
    {
        while (liste != NULL)
        {
            if (liste != entity && liste->type == type)
            {
                for (i = 0; i < liste->nb_segments; i++ )
                {
                    if (sat_seg(entity, liste->hitbox[i]))
                    {
                        return SDL_TRUE;
                    }
                }
            }
            liste = liste->suivant;
        }
        return SDL_FALSE;
    }
    else
    {
        while (liste != NULL)
        {
            if (liste != entity && liste->type == type)
            {
                if (sat(entity, liste))
                    return SDL_TRUE;
            }
            liste = liste->suivant;
        }
        return SDL_FALSE;
    }
}

void move_Entity(Entity *entity, int x, int y)
{
    entity->x += x;
    entity->y += y;
    entity->target_render_texture.x += x;
    entity->target_render_texture.y += y;
    for (int i = 0; i < entity->nb_segments-1 ; i++)
    {
        entity->hitbox[i].A.x += x;
        entity->hitbox[i].A.y += y;
        entity->hitbox[i].B.x += x;
        entity->hitbox[i].B.y += y;
    }
}

void affiche_Entity(Entity *entity, SDL_Renderer *renderer)
{
    if (entity->texture != NULL)
    {
        SDL_RenderCopy(renderer, entity->texture, &entity->src_render_texture, &entity->target_render_texture);
    }
}

void load_Room(const char chemin[], SDL_Renderer *renderer, Entity *liste)
{
    FILE *fichier = NULL;
    int nb_entity, x, y, i;

    fichier = fopen(chemin, "r");
    if (fichier != NULL)
    {
        fscanf(fichier,"%d ", &nb_entity);
        for (i = 0; i < nb_entity; i++)
        {
            char chemin_entity[41];
            Entity *entity = NULL;
            fscanf(fichier,"%s %d %d ", chemin_entity, &x, &y);
            entity = load_Entity(chemin_entity, renderer, liste);
            move_Entity(entity, x, y);
        }
        fclose(fichier);
    }
    else
        printf("L'ouverture du fichier %s a échouée\n", chemin);
}

void jump_fall_Entity(Entity *entity, Entity *liste_entity)
{
    SDL_bool is_collision = SDL_FALSE;

    if (!entity->jumping)
    {
        for (int i = 0; i < entity->fall_speed; i++)
        {
            move_Entity(entity, 0, 1);
            is_collision = collision(entity, 2, liste_entity);
            if (is_collision)
            {
                move_Entity(entity, 0, -1);
                entity->on_ground = SDL_TRUE;
                if (!entity->jump_command_down)
                {
                    entity->can_jump = SDL_TRUE;
                }
                entity->jump_high = 0;
                entity->fall_high = 0;
                entity->fall_speed = 1;
            }
            else
            {
                entity->fall_high += 1;
                entity->on_ground = SDL_FALSE;
                entity->can_jump = SDL_FALSE;
                if (entity->fall_high > 10)
                {
                    entity->fall_speed = 2;
                }
                if (entity->fall_high > 20)
                {
                    entity->fall_speed = 3;
                }
                if (entity->fall_high > 30)
                {
                    entity->fall_speed = 4;
                }
            }
        }
    }

    if (entity->jump_command_down)
    {
        if (entity->can_jump || entity->jumping)
            {
                for (int i = 0; i < entity->jump_speed; i++)
                {
                    if (entity->jump_high >= entity->jump_high_max)
                    {
                        entity->jumping = SDL_FALSE;
                        entity->jump_speed = 3;
                    }
                    else
                    {
                        entity->jumping = SDL_TRUE;
                        entity->on_ground = SDL_FALSE;
                        entity->can_jump = SDL_FALSE;
                        entity->jump_high += 1;
                        move_Entity(entity, 0, -1);
                        is_collision = collision(entity, 2, liste_entity);
                        if (is_collision)
                        {
                            move_Entity(entity, 0, 1);
                            entity->jumping = SDL_FALSE;
                            entity->jump_speed = 3;
                        }
                        if (entity->jump_high > entity->jump_high_max - 20)
                        {
                            entity->jump_speed = 2;
                        }
                        if (entity->jump_high > entity->jump_high_max - 10)
                        {
                            entity->jump_speed = 1;
                        }
                    }
                }
            }
    }
    else
    {
        entity->jumping = SDL_FALSE;
        entity->jump_speed = 3;
    }
}

void try_move_Entity_x(Entity *entity, SDL_bool x_positif, Entity *liste_entity)
{
    SDL_bool is_collision = SDL_FALSE;
    if (x_positif)
    {
        entity->left_dist = 0;
        if (entity->on_ground)
        {
            move_Entity(entity, 1, 1);
            entity->right_dist += 1;
            is_collision = collision(entity, 2, liste_entity);
            if (is_collision)
            {
                move_Entity(entity, -1, -1);
                move_Entity(entity, 1, 0);
                is_collision = collision(entity, 2, liste_entity);
                if (is_collision)
                {
                    move_Entity(entity, -1, 0);
                    move_Entity(entity, 1, -1);
                    is_collision = collision(entity, 2, liste_entity);
                    if (is_collision)
                    {
                        move_Entity(entity, -1, 1);
                        entity->right_dist = 0;
                    }
                }
            }
        }
        else
        {
            move_Entity(entity, 1, 0);
            entity->right_dist += 1;
            is_collision = collision(entity, 2, liste_entity);
            if (is_collision)
            {
                move_Entity(entity, -1, 0);
                move_Entity(entity, 1, 3);
                is_collision = collision(entity, 2, liste_entity);
                if (is_collision)
                {
                    move_Entity(entity, -1, -3);
                    move_Entity(entity, 1, -3);
                    is_collision = collision(entity, 2, liste_entity);
                    if (is_collision)
                    {
                        move_Entity(entity, -1, 3);
                        entity->right_dist = 0;
                    }
                }
            }
        }
    }
    else
    {
        entity->right_dist = 0;
        if (entity->on_ground)
        {
            move_Entity(entity, -1, 1);
            entity->left_dist += 1;
            is_collision = collision(entity, 2, liste_entity);
            if (is_collision)
            {
                move_Entity(entity, 1, -1);
                move_Entity(entity, -1, 0);
                is_collision = collision(entity, 2, liste_entity);
                if (is_collision)
                {
                    move_Entity(entity, 1, 0);
                    move_Entity(entity, -1, -1);
                    is_collision = collision(entity, 2, liste_entity);
                    if (is_collision)
                    {
                        move_Entity(entity, 1, 1);
                        entity->left_dist = 0;
                    }
                }
            }
        }
        else
        {
            move_Entity(entity, -1, 0);
            entity->left_dist += 1;
            is_collision = collision(entity, 2, liste_entity);
            if (is_collision)
            {
                move_Entity(entity, 1, 0);
                move_Entity(entity, -1, 3);
                is_collision = collision(entity, 2, liste_entity);
                if (is_collision)
                {
                    move_Entity(entity, 1, -3);
                    move_Entity(entity, -1, -3);
                    is_collision = collision(entity, 2, liste_entity);
                    if (is_collision)
                    {
                        move_Entity(entity, 1, 3);
                        entity->left_dist = 0;
                    }
                }
            }
        }
    }
}

void rl_run_Entity(Entity *entity, Entity *liste_entity)
{
    if (entity->left_command_down)
    {
        for (int i = 0; i < entity->speed; i++)
        {
            if (entity->left_dist < 15)
            {
                entity->speed = 1;
            }
            if (entity->left_dist >= 15 && entity->left_dist < 40)
            {
                entity->speed = 2;
            }
            if (entity->left_dist >= 40)
            {
                entity->speed = 2;
            }
            try_move_Entity_x(entity, SDL_FALSE, liste_entity);
        }
    }
    if (entity->right_command_down)
    {
        for (int i = 0; i < entity->speed; i++)
        {
            if (entity->right_dist < 15)
            {
                entity->speed = 1;
            }
            if (entity->right_dist >= 15 && entity->right_dist < 40)
            {
                entity->speed = 2;
            }
            if (entity->right_dist >= 40)
            {
                entity->speed = 2;
            }
            try_move_Entity_x(entity, SDL_TRUE, liste_entity);
        }
    }
    if (!(entity->right_command_down || entity->left_command_down))
    {
        entity->speed = 1;
        entity->left_dist = 0;
        entity->right_dist = 0;
    }

}

void update_Player(Entity *player, Input input, Entity *liste_entity, SDL_Renderer *renderer)
{
    player->left_command_down = input.key[SDL_GetScancodeFromKey(SDLK_LEFT)];
    player->right_command_down = input.key[SDL_GetScancodeFromKey(SDLK_RIGHT)];
    rl_run_Entity(player, liste_entity);
    player->jump_command_down = input.key[SDL_GetScancodeFromKey(SDLK_SPACE)];
    jump_fall_Entity(player, liste_entity);
    affiche_Entity(player, renderer);
}

int main(int argc, char *argv[])
{
    /* Création des variables */

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Surface *icone = NULL;
    int statut = EXIT_FAILURE;
    Entity liste_entity = {.type = 0, .suivant = NULL};
    Game game = {.game_state = 0};
    Input input = {.quit = SDL_FALSE};
    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
        input.key[i] = SDL_FALSE;

    /* Initialisation, création de la fenêtre et du renderer. */

    Init(&window, &renderer, &icone, &liste_entity, statut);

    /* Boucle principale du jeu */

    Entity *test = load_Entity("entity/test/test.dat", renderer, &liste_entity);
    test->affected_by_gravity = SDL_TRUE;
    test->can_jump = SDL_TRUE;
    test->on_ground = SDL_TRUE;
    test->jumping = SDL_FALSE;
    test->jump_high = 0;
    test->jump_high_max = 80;
    test->jump_speed = 3;
    test->fall_high = 0;
    test->fall_speed = 1;
    test->left_dist = 0;
    test->right_dist = 0;
    test->speed = 1;
    test->speed_max = 3;
    test->jump_command_down = SDL_FALSE;
    
    move_Entity(test, 240, 135);
    affiche_Entity(test, renderer);
    Entity *room = load_Entity("entity/save_room/save_room.dat", renderer, &liste_entity);
    
    /*FILE *fichier = NULL;
    fichier = fopen("entity/test/test.dat", "w");
    if (fichier != NULL)
    {
        fprintf(fichier, "%d %s %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                        0, "entity/test/texture.bmp", 1, 20, 20, -10, -10, 4, "none", 0, 0, 0, 0, 4, -3, -3, 3, -3, 10, 3, -3, 3, 3, 11, 3, 3, -3, 3, 9, -3, 3, -3, -3, 12);
        fclose(fichier);
    }*/

    while (!input.quit)
    {
        updateEvent(&input);
        if (game.game_state == 0)
        {
            affiche_Entity(room, renderer);

            update_Player(test, input, &liste_entity, renderer);
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    /* Fermeture du logiciel et libération de la mémoire */

    statut = EXIT_SUCCESS;
    Quit(window, renderer, icone, &liste_entity, statut);
}