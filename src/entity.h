#pragma once

#include <allegro5/allegro5.h>

typedef enum {
    Static, Kinematic
} entity_type_t;

typedef struct Vector_s {
    float x;
    float y;
} Vector;

typedef struct Entity_s {
    entity_type_t type;
    ALLEGRO_BITMAP* image;
    Vector position;
    Vector velocity;
} Entity;

Entity* entity_create(entity_type_t type, const char* image_filename, Vector position);

void entities_update();
void entity_render(Entity* entity);