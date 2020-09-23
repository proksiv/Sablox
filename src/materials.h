#pragma once

#include <allegro5/allegro5.h>

typedef enum
{
    Air, Stone, Sand, Wood, Fire, MaterialCount
} MATERIAL;

typedef struct
{
    const char* name;
    void (*update_routine)(int, int);
    ALLEGRO_COLOR color;
} MATERIAL_DATA;

void material_update_sand();
void material_update_fire();

void materials_init();

MATERIAL_DATA material_get_data(MATERIAL m);