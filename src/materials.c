#include "materials.h"
#include "world.h"

MATERIAL_DATA materials_data[MaterialCount];

void material_update_sand(int x, int y)
{
    if(world_get_cell(x, y + 1))
    {
        MATERIAL free_dl = !world_get_cell(x - 1, y + 1);
        MATERIAL free_dr = !world_get_cell(x + 1, y + 1);

        if(steps % 2 && free_dr)
            world_move_cell(x, y, x + 1, y + 1);
        else if(free_dl)
            world_move_cell(x, y, x - 1, y + 1);
        else if(free_dr)
            world_move_cell(x, y, x + 1, y + 1);
    }
    else
        world_move_cell(x, y, x, y + 1);
}

void material_update_fire(int x, int y)
{
    int k, l;

    for(k = x - 1; k <= x + 1; k++)
    {
        for(l = y - 1; l <= y + 1; l++)
        {
            if(k == x && l == y)
                continue;
            if(world_get_cell(k, l) == Wood)
            {
                world_set_cell(k, l, Fire);
                world_set_updated(k, l);
            }
        }
    }
    world_set_cell(x, y, Air);
}

void materials_init()
{
    MATERIAL_DATA data;

    data.name = "Air";
    data.update_routine = NULL;
    data.color = al_map_rgb_f(0.0, 0.0, 0.0);
    materials_data[Air] = data;

    data.name = "Stone";
    data.update_routine = NULL;
    data.color = al_map_rgb_f(0.5, 0.5, 0.5);
    materials_data[Stone] = data;

    data.name = "Sand";
    data.update_routine = &material_update_sand;
    data.color = al_map_rgb_f(0.7, 0.7, 0.5);
    materials_data[Sand] = data;

    data.name = "Wood";
    data.update_routine = NULL;
    data.color = al_map_rgb_f(0.5, 0.3, 0.2);
    materials_data[Wood] = data;

    data.name = "Fire";
    data.update_routine = &material_update_fire;
    data.color = al_map_rgb_f(0.9, 0.3, 0.1);
    materials_data[Fire] = data;
}

MATERIAL_DATA material_get_data(MATERIAL m)
{
    return materials_data[m];
}