#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include "world.h"

#define CELLS_TO_RENDER (WORLD_W - 2)*(WORLD_H - 2)
const int RENDER_H = WORLD_H - 2;

typedef struct {
    MATERIAL m;
    lifetime_t lifetime;
    ALLEGRO_COLOR color;
} CELL;

CELL world[WORLD_W][WORLD_H];

ALLEGRO_VERTEX varray[CELLS_TO_RENDER*6];
ALLEGRO_VERTEX_BUFFER* vbuf;

/* Private functions */

static void world_redraw_cell(int cell_x, int cell_y)
{
    if (cell_x <= 0 || cell_y <= 0 || cell_x >= WORLD_W - 1 || cell_y >= WORLD_H - 1)
        return;

    int index = ((cell_x - 1)*RENDER_H + (cell_y - 1))*6;
    void* lock_mem = al_lock_vertex_buffer(vbuf, index, 6, ALLEGRO_LOCK_WRITEONLY);
    varray[index].color = varray[index + 1].color = varray[index + 2].color = varray[index + 3].color = varray[index + 4].color
        = varray[index + 5].color = world[cell_x][cell_y].color;
    memcpy(lock_mem, varray + index, sizeof(ALLEGRO_VERTEX)*6);
    al_unlock_vertex_buffer(vbuf);
}

static void world_check_cell_lifetime(int cell_x, int cell_y)
{
    CELL* cell = &(world[cell_x][cell_y]);
    cell->lifetime--;
    if(cell->lifetime == 0)
        world_set_cell_material(cell_x, cell_y, Air);
}

static float clamp(float value, float min, float max)
{
    return value < min ? min : value > max ? max : value;
}

/* Public functions */

void world_init()
{
    materials_init();
    memset(&world, 0, sizeof(world));
    steps = 0;
    
    int i, cell_x, cell_y;
    for(i = 0; i < CELLS_TO_RENDER; i++)
    {
        cell_x = i/RENDER_H;
        cell_y = i%RENDER_H;
        ALLEGRO_VERTEX* v = &varray[i*6];
        v[0].x = v[2].x = v[3].x = cell_x;
        v[0].y = v[1].y = v[5].y = cell_y;
        v[1].x = v[4].x = v[5].x = cell_x + 1;
        v[2].y = v[3].y = v[4].y = cell_y + 1;
        v[0].z = v[1].z = v[2].z = v[3].z = v[4].z = v[5].z = 0;
        v[0].color = v[1].color = v[2].color = v[3].color = v[4].color = v[5].color = material_get_data(Air).color;
    }

    vbuf = al_create_vertex_buffer(NULL, varray, CELLS_TO_RENDER*6, ALLEGRO_PRIM_BUFFER_STREAM);
}

bool world_get_cell_updated(int cell_x, int cell_y)
{
    return (world[cell_x][cell_y].m >> 7) & 1;
}

void world_set_cell_updated(int cell_x, int cell_y)
{
    if(steps % 2)
        world[cell_x][cell_y].m |= (1 << 7);
    else
        world[cell_x][cell_y].m &= (~(1 << 7));
}

MATERIAL world_get_cell_material(int cell_x, int cell_y)
{
    return (world[cell_x][cell_y].m & 63);
}

void world_set_cell_material(int cell_x, int cell_y, MATERIAL m)
{
    world[cell_x][cell_y].m = (world_get_cell_updated(cell_x, cell_y) << 7) | m;
    world_set_cell_lifetime(cell_x, cell_y, material_get_data(m).initial_lifetime);

    ALLEGRO_COLOR color = material_get_data(world_get_cell_material(cell_x, cell_y)).color;
    if(color.a > 0)
    {
        color.r = clamp(color.r - 0.05 + (float)rand()/(float)(RAND_MAX/0.1), 0.0, 1.0);
        color.g = clamp(color.g - 0.05 + (float)rand()/(float)(RAND_MAX/0.1), 0.0, 1.0);
        color.b = clamp(color.b - 0.05 + (float)rand()/(float)(RAND_MAX/0.1), 0.0, 1.0);
    }
    world[cell_x][cell_y].color = color;

    world_redraw_cell(cell_x, cell_y);
}

lifetime_t world_get_cell_lifetime(int cell_x, int cell_y)
{
    return world[cell_x][cell_y].lifetime;
}

void world_set_cell_lifetime(int cell_x, int cell_y, lifetime_t lifetime)
{
    world[cell_x][cell_y].lifetime = lifetime;
}

bool world_move_cell(int src_x, int src_y, int dst_x, int dst_y)
{
    if(world_get_cell_material(dst_x, dst_y))
        return false;

    memcpy(&(world[dst_x][dst_y]), &(world[src_x][src_y]), sizeof(CELL));
    world_redraw_cell(dst_x, dst_y);
    world_set_cell_updated(dst_x, dst_y);

    world_set_cell_material(src_x, src_y, Air);
    return true;
}

void world_swap_cells(int ax, int ay, int bx, int by)
{
    if(material_get_data(world_get_cell_material(ax, ay)).density < material_get_data(world_get_cell_material(bx, by)).density)
        return;

    CELL temp = world[ax][ay];
    memcpy(&(world[ax][ay]), &(world[bx][by]), sizeof(CELL));
    world_redraw_cell(ax, ay);
    world_set_cell_updated(ax, ay);

    memcpy(&(world[bx][by]), &temp, sizeof(CELL));
    world_redraw_cell(bx, by);
    world_set_cell_updated(bx, by);
}

void world_paint(int cell_x, int cell_y, MATERIAL m)
{
    if (cell_x < 0 || cell_y < 0 || cell_x >= WORLD_W || cell_y >= WORLD_H)
        return;

    int i, j;
    for(i = cell_x - 1; i <= cell_x + 1; i++)
    {
        for(j = cell_y - 1; j <= cell_y + 1; j++)
        {
            if(i != cell_x && j != cell_y)
                continue;
            world_set_cell_material(i, j, m);
        }
    }
}

void material_postcheck_density(int x, int y);
void material_postupdate_fluid(int x, int y);
void world_update()
{
    int i, j;
    for(i = 1; i < WORLD_W - 1; i++)
    {
        for(j = 1; j < WORLD_H - 1; j++)
        {
            if(world_get_cell_updated(i, j) == (steps % 2))
                continue;

            MATERIAL_DATA data = material_get_data(world_get_cell_material(i, j));

            if(data.initial_lifetime > 0)
                world_check_cell_lifetime(i, j);

            if(data.update_routine != NULL)
                data.update_routine(i, j);

            if(data.is_fluid)
                material_postupdate_fluid(i, j);

            material_postcheck_density(i, j);
        }
    }

    steps++;
}

void world_render()
{
    al_draw_vertex_buffer(vbuf, NULL, 0, CELLS_TO_RENDER*6, ALLEGRO_PRIM_TRIANGLE_LIST);
}