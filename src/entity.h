#include <allegro5/allegro5.h>

typedef struct Vector_s {
    float x;
    float y;
} Vector;

typedef union EntityImage_u {
    ALLEGRO_BITMAP* sprite;
    ALLEGRO_COLOR color;
} EntityImage;

typedef struct Entity_s {
    Vector position;
    Vector velocity;
    EntityImage image;
    bool use_bitmap;
} Entity;

Entity* entity_create();

void entities_update();
void entity_render(Entity* entity);