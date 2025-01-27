#include "player.h"

void load_Player(Player *player, SDL_Renderer *renderer)
{
    player->hitbox.points[0].x = 0;
    player->hitbox.points[0].y = 0;
    player->hitbox.points[1].x = 16;
    player->hitbox.points[1].y = 0;
    player->hitbox.points[2].x = 16;
    player->hitbox.points[2].y = 32;
    player->hitbox.points[3].x = 0;
    player->hitbox.points[3].y = 32;
    player->nb_col = 0;
    player->nb_row = 0;
    player->texture = NULL;
    player->src.x = 0;
    player->src.y = 0;
    player->src.w = 16;
    player->src.h = 32;
    player->dst = player->src;
}

void destroy_Player(Player *player)
{
    if (player->texture != NULL)
    {
        SDL_DestroyTexture(player->texture);
    }
}

void move_Player(int x, int y, Player *player, Level *level)
{
    move_Hitbox(x, y, &player->hitbox);
    Vector2 mtv = sat_bulk(&player->hitbox, level->hitboxes, level->nb_hitboxes);
    if (mtv.x != 0 || mtv.y != 0)
    {
        move_Hitbox(mtv.x, mtv.y, &player->hitbox);
    }
    int mid_x = (player->hitbox.points[0].x + player->hitbox.points[1].x) / 2;
    int mid_y = (player->hitbox.points[0].y + player->hitbox.points[3].y) / 2;
    mtv = move_cam_Level(-level->src.x + mid_x -level->src.w / 2, -level->src.y + mid_y -level->src.h / 2, level);
    player->dst.x = mtv.x + level->src.w / 2 -player->dst.w / 2;
    player->dst.y = mtv.y + level->src.h / 2 -player->dst.h / 2;
}

void display_Player(Player *player, SDL_Renderer *renderer)
{
    render_Texture(renderer, player->texture, &player->src, &player->dst);
}

void animate_Player(Player *player)
{
    if (player->src.x + player->src.w >= player->src.w * player->nb_col)
    {
        player->src.x = 0;
    }
    else
    {
        player->src.x += player->src.w;
    }
}

void chg_animation_Player(int animation_line, Player *player)
{
    if (animation_line + 1 <= player->nb_row && animation_line >= 0)
    {
        player->src.y = player->src.h * animation_line;
        player->src.x = 0;
    }
}

void update_Game(Player *player, Input *input, Level *level, SDL_Renderer *renderer)
{
    if (input->right)
    {
        move_Player(2, 0, player, level);
    }
    if (input->left)
    {
        move_Player(-2, 0, player, level);
    }
    if (input->up)
    {
        move_Player(0, -2, player, level);
    }
    if (input->down)
    {
        move_Player(0, 2, player, level);
    }
    display_Level(level, renderer);
    display_Hitbox(&player->hitbox, - player->hitbox.points[0].x + player->dst.x, - player->hitbox.points[0].y + player->dst.y, 255, 0, 0, renderer);
}