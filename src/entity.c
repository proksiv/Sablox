#include <stddef.h>
#include <stdlib.h>

#include "globals.h"
#include "entity.h"

typedef struct EntityNode_s {
    Entity entity;
    struct EntityNode_s* next;
} EntityNode;

EntityNode* entity_head = NULL;

void world_set_cell_material(int cell_x, int cell_y, int m);
void world_set_cell_color(int cell_x, int cell_y, ALLEGRO_COLOR color);
Entity* entity_create(ENTITY_TYPE type, const char* image_filename, Vector position)
{
    EntityNode* new_node = malloc(sizeof(EntityNode));

    new_node->entity.type = type;
    new_node->entity.position = position;
    new_node->entity.velocity = (Vector){.y = 0, .y = 0};

    int default_flags = al_get_new_bitmap_flags();
    al_set_path_filename(path, image_filename);
    if(type == Static)
        al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    new_node->entity.image = al_load_bitmap(al_path_cstr(path, '/'));
    al_set_new_bitmap_flags(default_flags);

    if(type == Static)
    {
        int w = al_get_bitmap_width(new_node->entity.image);
        int h = al_get_bitmap_height(new_node->entity.image);
        ALLEGRO_COLOR color;

        int i, j;
        for(i = 0; i < w; i++)
        {
            for(j = 0; j < h; j++)
            {
                world_set_cell_material((int)(new_node->entity.position.x) + i,(int)(new_node->entity.position.y) + j, 12);
                color = al_get_pixel(new_node->entity.image, i, j);
                world_set_cell_color((int)(new_node->entity.position.x) + i,(int)(new_node->entity.position.y) + j, color);
            }
        }

        al_destroy_bitmap(new_node->entity.image);
    }

    new_node->next = entity_head;
    entity_head = new_node;

    return &(new_node->entity);
}

void entity_update(Entity* entity)
{
    if(entity->type == Kinematic)
    {
        entity->position.x += entity->velocity.x;
        entity->position.y += entity->velocity.y;
    }
}

void entity_render(Entity* entity)
{
    if(entity->type == Kinematic)
    {

    }
}

void entities_update()
{
    EntityNode* iterator = entity_head;
    while(iterator != NULL)
    {
        entity_update(&(iterator->entity));
        iterator = iterator->next;
    }
}

void entities_render()
{
    EntityNode* iterator = entity_head;
    while(iterator != NULL)
    {
        entity_render(&(iterator->entity));
        iterator = iterator->next;
    }
}