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
    return;
}

void move_Player(int x, int y, Player *player, Level *level)
{
    SDL_bool osef, move_x, move_y, mid_x = (player->dst.x + player->dst.w / 2 == 160), mid_y = (player->dst.y + player->dst.h / 2 == 120);
    move_Hitbox(x, y, &player->hitbox);
    if (collision_Level(&player->hitbox, level))
    {
        move_Hitbox(-x, -y, &player->hitbox);
        return;
    }
    if (mid_x)
    {
        move_cam_Level(x, 0, &move_x, &osef, level);
    }
    if (mid_y)
    {
        move_cam_Level(0, y, &osef, &move_y, level);
    }
    if (!mid_x || !move_x)
    {
        player->dst.x += x;
    }
    if (!mid_y || !move_y)
    {
        player->dst.y += y;
    }
}

void display_Player(Player *player, SDL_Renderer *renderer)
{
    if (player->texture != NULL)
    {
        SDL_RenderCopy(renderer, player->texture, &player->src, &player->dst);
    }
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
    display_Hitbox(&player->hitbox, - player->hitbox.points[0].x + player->dst.x, - player->hitbox.points[0].y + player->dst.y, renderer);
}