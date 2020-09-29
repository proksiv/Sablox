#include "materials.h"
#include "world.h"

MATERIAL_DATA materials_data[MaterialCount];

void material_check_density(int x, int y)
{
    MATERIAL_DATA self = material_get_data(world_get_cell_material(x, y));
    MATERIAL_DATA below = material_get_data(world_get_cell_material(x, y + 1));
    if(below.density > 0 && self.density > below.density)
        world_swap_cells(x, y, x, y + 1);
}

void material_update_sand(int x, int y)
{
    if(world_get_cell_material(x, y + 1))
    {
        MATERIAL free_dl = !world_get_cell_material(x - 1, y + 1);
        MATERIAL free_dr = !world_get_cell_material(x + 1, y + 1);

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
            if(world_get_cell_material(k, l) == Wood)
            {
                world_set_cell_material(k, l, Fire);
                world_set_cell_updated(k, l);
            }
        }
    }
    world_set_cell_material(x, y, Smoke);
}

void material_update_water(int x, int y)
{
    if(world_get_cell_material(x, y + 1))
    {
        MATERIAL free_dl = !world_get_cell_material(x - 1, y + 1);
        MATERIAL free_dr = !world_get_cell_material(x + 1, y + 1);
        MATERIAL free_l = !world_get_cell_material(x - 1, y);
        MATERIAL free_r = !world_get_cell_material(x + 1, y);

        if(steps % 2 && free_dr)
            world_move_cell(x, y, x + 1, y + 1);
        else if(free_dl)
            world_move_cell(x, y, x - 1, y + 1);
        else if(free_dr)
            world_move_cell(x, y, x + 1, y + 1);
        else if(steps % 2 && free_r && !free_l)
            world_move_cell(x, y, x + 1, y);
        else if (free_l && !free_r)
            world_move_cell(x, y, x - 1, y);
        else if (free_r && !free_l)
            world_move_cell(x, y, x + 1, y);
    }
    else
        world_move_cell(x, y, x, y + 1);
}

void material_update_acid(int x, int y)
{
    int k, l;
    bool consumed = false;

    for(k = x - 1; k <= x + 1; k++)
    {
        for(l = y; l <= y + 1; l++)
        {
            if(k == x && l == y)
                continue;
            MATERIAL target = world_get_cell_material(k, l);
            if(target != Acid && target != Air)
            {
                world_set_cell_material(k, l, Air);
                world_set_cell_updated(k, l);
                consumed = true;
            }
        }
    }
    if(consumed)
        world_set_cell_material(x, y, Air);

    material_update_water(x, y);
}

void material_update_smoke(int x, int y)
{
    if(world_get_cell_material(x, y - 1))
    {
        MATERIAL free_ul = !world_get_cell_material(x - 1, y - 1);
        MATERIAL free_ur = !world_get_cell_material(x + 1, y - 1);

        if(steps % 2 && free_ur)
            world_move_cell(x, y, x + 1, y - 1);
        else if(free_ul)
            world_move_cell(x, y, x - 1, y - 1);
        else if(free_ur)
            world_move_cell(x, y, x + 1, y - 1);
    }
    else
        world_move_cell(x, y, x, y - 1);
}

void materials_init()
{
    MATERIAL_DATA data;

    data.name = "Air";
    data.update_routine = NULL;
    data.color = al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
    data.density = 0;
    materials_data[Air] = data;

    data.name = "Stone";
    data.update_routine = NULL;
    data.color = al_map_rgb_f(0.5, 0.5, 0.5);
    data.density = 0;
    materials_data[Stone] = data;

    data.name = "Sand";
    data.update_routine = &material_update_sand;
    data.color = al_map_rgb_f(0.7, 0.7, 0.5);
    data.density = 1442;
    materials_data[Sand] = data;

    data.name = "Wood";
    data.update_routine = NULL;
    data.color = al_map_rgb_f(0.5, 0.3, 0.2);
    data.density = 0;
    materials_data[Wood] = data;

    data.name = "Fire";
    data.update_routine = &material_update_fire;
    data.color = al_map_rgb_f(0.9, 0.3, 0.1);
    data.density = 0;
    materials_data[Fire] = data;

    data.name = "Water";
    data.update_routine = &material_update_water;
    data.color = al_map_rgb_f(0.1, 0.5, 0.9);
    data.density = 1000;
    materials_data[Water] = data;

    data.name = "Acid";
    data.update_routine = &material_update_acid;
    data.color = al_map_rgb_f(0.7, 0.8, 0.1);
    data.density = 1200;
    materials_data[Acid] = data;

    data.name = "Smoke";
    data.update_routine = &material_update_smoke;
    data.color = al_map_rgb_f(0.3, 0.3, 0.3);
    data.density = 1;
    materials_data[Smoke] = data;
}

MATERIAL_DATA material_get_data(MATERIAL m)
{
    return materials_data[m];
}