#pragma once

#include <allegro5/allegro5.h>

typedef unsigned char lifetime_t;
typedef unsigned char hardness_t;

typedef enum
{
    Air, Stone, Sand, Wood, Fire, Water, Acid, Smoke, Ember, Oil, Obsidian, Lava, MaterialCount
} MATERIAL;

typedef struct
{
    const char* name;
    void (*update_routine)(int, int);
    ALLEGRO_COLOR color;
    unsigned int density;
    lifetime_t initial_lifetime;
    bool is_fluid;
    hardness_t hardness;
} MATERIAL_DATA;

void materials_init();

MATERIAL_DATA material_get_data(MATERIAL m);