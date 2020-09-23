#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include "world.h"

char world[WORLD_W][WORLD_H];

void world_init()
{
    materials_init();
    memset(world, Air, WORLD_W*WORLD_H);
    steps = 0;
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
    void (*update_routine)(int, int);

    int i, j;
    for(i = 1; i < WORLD_W - 1; i++)
    {
        for(j = 1; j < WORLD_H - 1; j++)
        {
            if(world_get_updated(i, j) == (steps % 2))
                continue;

            update_routine = material_get_data(world_get_cell(i, j)).update_routine;
            if (update_routine != NULL)
                update_routine(i, j);
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
            color = material_get_data(world_get_cell(i, j)).color;
            unsigned char r, g, b;
            al_unmap_rgb(color, &r, &g, &b);
            if(r == 0 && g == 0 && b == 0)
                continue;

            float x1 = (i - 1)*4;
            float y1 = (j - 1)*4;
            float x2 = x1 + 4;
            float y2 = y1 + 4;
            al_draw_filled_rectangle(x1, y1, x2, y2, color);
        }
    }
}