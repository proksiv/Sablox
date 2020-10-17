#pragma once

#include <allegro5/allegro5.h>

typedef unsigned char lifetime_t;
typedef unsigned char hardness_t;

typedef enum
{
    AIR, STONE, SAND, WOOD, FIRE, WATER, ACID, SMOKE, EMBER, OIL, OBSIDIAN, LAVA, ENTITY_FRAGMENT, MATERIAL_COUNT
} material_t;

typedef struct MaterialData_s
{
    const char* name;
    void (*update_routine)(int, int);
    ALLEGRO_COLOR color;
    unsigned int density;
    lifetime_t initial_lifetime;
    bool is_fluid;
    hardness_t hardness;
    bool use_noise;
} MaterialData;

void materials_init();

MaterialData material_get_data(material_t m);