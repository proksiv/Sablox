#pragma once

#include <allegro5/allegro5.h>

typedef enum
{
    Air, Stone, Sand, Wood, Fire, Water, Acid, MaterialCount
} MATERIAL;

typedef struct
{
    const char* name;
    void (*update_routine)(int, int);
    ALLEGRO_COLOR color;
    unsigned int density;
} MATERIAL_DATA;

void material_check_density(int x, int y);

void material_update_sand(int x, int y);
void material_update_fire(int x, int y);
void material_update_water(int x, int y);
void material_update_acid(int x, int y);

void materials_init();

MATERIAL_DATA material_get_data(MATERIAL m);