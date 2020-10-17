#pragma once

#include <allegro5/allegro5.h>

typedef enum {
    Static, Kinematic
} ENTITY_TYPE;

typedef struct Vector_s {
    float x;
    float y;
} Vector;

typedef struct Entity_s {
    ENTITY_TYPE type;
    ALLEGRO_BITMAP* image;
    Vector position;
    Vector velocity;
} Entity;

Entity* entity_create(ENTITY_TYPE type, const char* image_filename, Vector position);

void entities_update();
void entity_render(Entity* entity);