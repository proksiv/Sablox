#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include "world.h"

#define CELLS_NUMBER WORLD_W*WORLD_H
#define CELLS_TO_RENDER (WORLD_W - 2)*(WORLD_H - 2)
#define CELL_SIZE 4

char world[WORLD_W][WORLD_H];

ALLEGRO_VERTEX varray[CELLS_TO_RENDER*6];
ALLEGRO_VERTEX_BUFFER* vbuf;

void world_init()
{
    MATERIAL m = Air;

    materials_init();
    memset(world, m, CELLS_NUMBER);
    steps = 0;
    
    int i, cell_x, cell_y;
    for(i = 0; i < CELLS_TO_RENDER; i++)
    {
        cell_x = i/WORLD_H;
        cell_y = i%WORLD_H;
        ALLEGRO_VERTEX* v = &varray[i*6];
        v[0].x = v[2].x = v[3].x = cell_x*CELL_SIZE;
        v[0].y = v[1].y = v[5].y = cell_y*CELL_SIZE;
        v[1].x = v[4].x = v[5].x = cell_x*CELL_SIZE + CELL_SIZE;
        v[2].y = v[3].y = v[4].y = cell_y*CELL_SIZE + CELL_SIZE;
        v[0].color = v[1].color = v[2].color = v[3].color = v[4].color = v[5].color = material_get_data(m).color;
    }

    vbuf = al_create_vertex_buffer(NULL, varray, CELLS_TO_RENDER*6, ALLEGRO_PRIM_TRIANGLE_LIST);
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

bool world_get_static(int cell_x, int cell_y)
{
    return (world[cell_x][cell_y] >> 6) & 1;
}

void world_set_static(int cell_x, int cell_y, bool is_static)
{
    if(is_static)
        world[cell_x][cell_y] |= (1 << 6);
    else
        world[cell_x][cell_y] &= (~(1 << 6));
}

MATERIAL world_get_cell(int cell_x, int cell_y)
{
    return (world[cell_x][cell_y] & 63);
}

void world_set_cell(int cell_x, int cell_y, MATERIAL m)
{
    world[cell_x][cell_y] = (world_get_updated(cell_x, cell_y) << 7) | (world_get_static(cell_x, cell_y) << 6) | m;

    int i, j;
    for(i = cell_x - 1; i <= cell_x; i++)
    {
        for(j = cell_y - 1; j <= cell_y; j++)
        {
            world_set_static(i, j, false);
        }
    }

    if (cell_x == 0 || cell_y == 0 || cell_x == WORLD_W - 1 || cell_y == WORLD_H - 1)
        return;

    int index = (--cell_x*WORLD_H + --cell_y)*6;
    void* lock_mem = al_lock_vertex_buffer(vbuf, index, 6, ALLEGRO_LOCK_WRITEONLY);
    varray[index].color = varray[index + 1].color = varray[index + 2].color = varray[index + 3].color = varray[index + 4].color = varray[index + 5].color = material_get_data(m).color;
    memcpy(lock_mem, varray + index, sizeof(ALLEGRO_VERTEX)*6);
    al_unlock_vertex_buffer(vbuf);
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
            if(world_get_static(i, j) || world_get_updated(i, j) == (steps % 2))
                continue;

            update_routine = material_get_data(world_get_cell(i, j)).update_routine;
            if (update_routine != NULL)
                update_routine(i, j);
            else
                world_set_static(i, j, true);
        }
    }

    steps++;
}

void world_render()
{
    al_draw_vertex_buffer(vbuf, NULL, 0, CELLS_TO_RENDER*6, ALLEGRO_PRIM_BUFFER_DYNAMIC);
}