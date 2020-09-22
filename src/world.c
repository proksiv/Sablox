#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include "world.h"

char world[WORLD_W][WORLD_H];
long steps = 0;

void world_init()
{
    memset(world, Air, WORLD_W*WORLD_H);
}

bool world_get_updated(int cell_x, int cell_y)
{
    return (world[cell_x][cell_y] >> 7) & 1;
}

void world_set_updated(int cell_x, int cell_y)
{
    if(steps % 2)
        world[cell_x][cell_y] |= (1 << 7);
    else
        world[cell_x][cell_y] &= (~(1 << 7));
}

MATERIAL world_get_cell(int cell_x, int cell_y)
{
    return (world[cell_x][cell_y] & 127);
}

void world_set_cell(int cell_x, int cell_y, MATERIAL m)
{
    world[cell_x][cell_y] = (world_get_updated(cell_x, cell_y) << 7) | m;
}

void world_paint(int cell_x, int cell_y, MATERIAL m)
{
    int i, j;
    for(i = cell_x - 1; i <= cell_x + 1; i++)
    {
        for(j = cell_y - 1; j <= cell_y + 1; j++)
        {
            if(i != cell_x && j != cell_y)
                continue;
            world_set_cell(i, j, m);
        }
    }
}

bool world_move_cell(int src_x, int src_y, int dst_x, int dst_y)
{
    if(world_get_cell(dst_x, dst_y))
        return false;

    world_set_cell(dst_x, dst_y, world_get_cell(src_x, src_y));
    world_set_updated(dst_x, dst_y);
    world_set_cell(src_x, src_y, Air);
    return true;
}

void world_update()
{
    int i, j, k, l;
    for(i = 1; i < WORLD_W - 1; i++)
    {
        for(j = 1; j < WORLD_H - 1; j++)
        {
            if(world_get_updated(i, j) == (steps % 2))
                continue;

            switch(world_get_cell(i, j))
            {
                case Sand:
                    if(world_get_cell(i, j + 1))
                    {
                        if(!world_get_cell(i - 1, j + 1))
                            world_move_cell(i, j, i - 1, j + 1);
                        else if(!world_get_cell(i + 1, j + 1))
                            world_move_cell(i, j, i + 1, j + 1);
                    }
                    else
                        world_move_cell(i, j, i, j + 1);
                    break;

                case Fire:
                    for(k = i - 1; k <= i + 1; k++)
                    {
                        for(l = j - 1; l <= j + 1; l++)
                        {
                            if(k == i && l == j)
                                continue;
                            if(world_get_cell(k, l) == Wood)
                            {
                                world_set_cell(k, l, Fire);
                                world_set_updated(k, l);
                            }
                        }
                    }
                    world_set_cell(i, j, Air);
                    break;

                default:
                    break;
            }
        }
    }

    steps++;
}

void world_render()
{
    ALLEGRO_COLOR color;

    int i, j;
    for(i = 1; i < WORLD_W - 1; i++)
    {
        for(j = 1; j < WORLD_H - 1; j++)
        {
            switch(world_get_cell(i, j))
            {
                case Stone:
                    color = al_map_rgb_f(0.5, 0.5, 0.5);
                    break;
                case Sand:
                    color = al_map_rgb_f(0.7, 0.7, 0.5);
                    break;
                case Wood:
                    color = al_map_rgb_f(0.5, 0.3, 0.2);
                    break;
                case Fire:
                    color = al_map_rgb_f(0.9, 0.3, 0.1);
                    break;
                default:
                    continue;
            }

            float x1 = (i - 1)*4;
            float y1 = (j - 1)*4;
            float x2 = x1 + 4;
            float y2 = y1 + 4;
            al_draw_filled_rectangle(x1, y1, x2, y2, color);
        }
    }
}