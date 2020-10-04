#include "materials.h"
#include "world.h"

long steps;

MATERIAL_DATA materials_data[MaterialCount];

/* Private functions */

void material_postcheck_density(int x, int y)
{
    if(world_get_cell_updated(x, y) == steps % 2)
        return;

    MATERIAL_DATA self = material_get_data(world_get_cell_material(x, y));
    MATERIAL_DATA below = material_get_data(world_get_cell_material(x, y + 1));
    if(below.density > 0 && self.density > below.density)
        world_swap_cells(x, y, x, y + 1);
}

void material_postupdate_fluid(int x, int y)
{
    if(world_get_cell_updated(x, y) == steps % 2)
        return;

    if(world_get_cell_material(x, y + 1))
    {
        MATERIAL self = world_get_cell_material(x, y);

        MATERIAL dl = world_get_cell_material(x - 1, y + 1);
        bool free_dl = !dl || (material_get_data(dl).is_fluid && self != dl);
        MATERIAL dr = world_get_cell_material(x + 1, y + 1);
        bool free_dr = !dr || (material_get_data(dr).is_fluid && self != dr);
        MATERIAL l = world_get_cell_material(x - 1, y);
        bool free_l = !l || (material_get_data(l).is_fluid && self != l);
        MATERIAL r = world_get_cell_material(x + 1, y);
        bool free_r = !r || (material_get_data(r).is_fluid && self != r);

        if(rand() % 2 && free_dr)
            world_swap_cells(x, y, x + 1, y + 1);
        else if(free_dl)
            world_swap_cells(x, y, x - 1, y + 1);
        else if(free_dr)
            world_swap_cells(x, y, x + 1, y + 1);
        else if(rand() % 2 && free_r && !free_l)
            world_swap_cells(x, y, x + 1, y);
        else if (free_l && !free_r)
            world_swap_cells(x, y, x - 1, y);
        else if (free_r && !free_l)
            world_swap_cells(x, y, x + 1, y);
    }
    else
        world_move_cell(x, y, x, y + 1);
}

static void material_update_sand(int x, int y)
{
    if(world_get_cell_material(x, y + 1))
    {
        MATERIAL free_dl = !world_get_cell_material(x - 1, y + 1);
        MATERIAL free_dr = !world_get_cell_material(x + 1, y + 1);

        if(rand() % 2 && free_dr)
            world_move_cell(x, y, x + 1, y + 1);
        else if(free_dl)
            world_move_cell(x, y, x - 1, y + 1);
        else if(free_dr)
            world_move_cell(x, y, x + 1, y + 1);
    }
    else
        world_move_cell(x, y, x, y + 1);
}

static void material_update_fire(int x, int y)
{
    int k, l;

    for(k = x - 1; k <= x + 1; k++)
    {
        for(l = y - 1; l <= y + 1; l++)
        {
            MATERIAL m = world_get_cell_material(k, l);
            if(m == Wood)
            {
                world_set_cell_material(k, l, Ember);
                world_set_cell_updated(k, l);
            }
            else if(m == Oil && rand() % 4 == 0)
            {
                world_set_cell_material(k, l, Fire);
                world_set_cell_updated(k, l);
                if(!world_get_cell_material(x, y - 1))
                    world_set_cell_material(x, y - 1, Smoke);
            }
        }
    }
    
    world_move_cell(x, y, x - 1 + rand() % 3, y - 1);
}

static void material_update_acid(int x, int y)
{
    int k, l;
    bool consumed = false;

    for(k = x - 1; k <= x + 1; k++)
    {
        for(l = y - 1; l <= y + 1; l++)
        {
            if(material_get_data(world_get_cell_material(k, l)).hardness < 255)
            {
                world_set_cell_material(k, l, Air);
                world_set_cell_updated(k, l);
                consumed = true;
            }
        }
    }
    
    if(consumed)
        world_set_cell_material(x, y, Air);
}

static void material_update_smoke(int x, int y)
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

static void material_update_ember(int x, int y)
{
    int k, l;

    if (rand() % 127 == 0)
    {
        for(k = x - 1; k <= x + 1; k++)
        {
            for(l = y - 1; l <= y + 1; l++)
            {
                if(world_get_cell_material(k, l) == Wood)
                {
                    world_set_cell_material(k, l, Ember);
                    world_set_cell_updated(k, l);
                }
                else if(world_get_cell_material(k, l) == Oil)
                {
                    world_set_cell_material(k, l, Fire);
                    world_set_cell_updated(k, l);
                }
            }
        }
    }

    if(!world_get_cell_material(x, y - 1))
    {
        if(rand() % (255 - world_get_cell_lifetime(x, y)) == 0)
            world_set_cell_material(x, y - 1, Fire);
        else if(rand() % 12 == 0)
            world_set_cell_material(x, y - 1, Smoke);
    }
}

static void material_update_lava(int x, int y)
{
    int k, l;

    for(k = x - 1; k <= x + 1; k++)
    {
        for(l = y - 1; l <= y + 1; l++)
        {
            MATERIAL m = world_get_cell_material(k, l);
            if(m == Wood)
            {
                world_set_cell_material(k, l, Ember);
                world_set_cell_updated(k, l);
            }
            else if(m == Oil)
            {
                world_set_cell_material(k, l, Fire);
                world_set_cell_updated(k, l);
                if(!world_get_cell_material(x, y - 1))
                    world_set_cell_material(x, y - 1, Smoke);
            }
            else if(m == Water)
                world_set_cell_material(x, y, Obsidian);
        }
    }
}

/* Public functions */

void materials_init()
{
    MATERIAL_DATA data;

    data.name = "Air";
    data.update_routine = NULL;
    data.color = al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
    data.density = 0;
    data.initial_lifetime = 0;
    data.is_fluid = false;
    data.hardness = 255;
    materials_data[Air] = data;

    data.name = "Stone";
    data.update_routine = NULL;
    data.color = al_map_rgb_f(0.5, 0.5, 0.5);
    data.density = 0;
    data.initial_lifetime = 0;
    data.is_fluid = false;
    data.hardness = 1;
    materials_data[Stone] = data;

    data.name = "Sand";
    data.update_routine = &material_update_sand;
    data.color = al_map_rgb_f(0.7, 0.7, 0.5);
    data.density = 1442;
    data.initial_lifetime = 0;
    data.is_fluid = false;
    data.hardness = 1;
    materials_data[Sand] = data;

    data.name = "Wood";
    data.update_routine = NULL;
    data.color = al_map_rgb_f(0.5, 0.3, 0.2);
    data.density = 0;
    data.initial_lifetime = 0;
    data.is_fluid = false;
    data.hardness = 1;
    materials_data[Wood] = data;

    data.name = "Fire";
    data.update_routine = &material_update_fire;
    data.color = al_map_rgb_f(0.9, 0.3, 0.1);
    data.density = 0;
    data.initial_lifetime = 12;
    data.is_fluid = false;
    data.hardness = 1;
    materials_data[Fire] = data;

    data.name = "Water";
    data.update_routine = NULL;
    data.color = al_map_rgb_f(0.1, 0.5, 0.9);
    data.density = 1000;
    data.initial_lifetime = 0;
    data.is_fluid = true;
    data.hardness = 1;
    materials_data[Water] = data;

    data.name = "Acid";
    data.update_routine = &material_update_acid;
    data.color = al_map_rgb_f(0.7, 0.8, 0.1);
    data.density = 1200;
    data.initial_lifetime = 0;
    data.is_fluid = true;
    data.hardness = 255;
    materials_data[Acid] = data;

    data.name = "Smoke";
    data.update_routine = &material_update_smoke;
    data.color = al_map_rgb_f(0.3, 0.3, 0.3);
    data.density = 1;
    data.initial_lifetime = 255;
    data.is_fluid = false;
    data.hardness = 1;
    materials_data[Smoke] = data;

    data.name = "Ember";
    data.update_routine = &material_update_ember;
    data.color = al_map_rgb_f(0.9, 0.4, 0.1);
    data.density = 0;
    data.initial_lifetime = 255;
    data.is_fluid = false;
    data.hardness = 1;
    materials_data[Ember] = data;

    data.name = "Oil";
    data.update_routine = NULL;
    data.color = al_map_rgb_f(0.7, 0.6, 0.4);
    data.density = 900;
    data.initial_lifetime = 0;
    data.is_fluid = true;
    data.hardness = 1;
    materials_data[Oil] = data;

    data.name = "Obsidian";
    data.update_routine = NULL;
    data.color = al_map_rgb_f(0.2, 0.2, 0.2);
    data.density = 0;
    data.initial_lifetime = 0;
    data.is_fluid = false;
    data.hardness = 255;
    materials_data[Obsidian] = data;

    data.name = "Lava";
    data.update_routine = &material_update_lava;
    data.color = al_map_rgb_f(0.8, 0.1, 0.1);
    data.density = 3100;
    data.initial_lifetime = 0;
    data.is_fluid = true;
    data.hardness = 255;
    materials_data[Lava] = data;
}

MATERIAL_DATA material_get_data(MATERIAL m)
{
    return materials_data[m];
}