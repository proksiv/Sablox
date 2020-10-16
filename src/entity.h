typedef struct Vector_s {
    float x;
    float y;
} Vector;

typedef struct Entity_s {
    Vector position;
    Vector velocity;
} Entity;

Entity* entity_create();

void entities_update();
void entities_render();