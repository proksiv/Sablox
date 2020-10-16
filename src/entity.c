#include <stddef.h>
#include <stdlib.h>

#include "entity.h"

typedef struct EntityNode_s {
    Entity entity;
    struct EntityNode_s* next;
} EntityNode;

EntityNode* entity_head = NULL;

Entity* entity_create()
{
    EntityNode* new_node = (EntityNode*)malloc(sizeof(EntityNode));

    new_node->entity.position = (Vector){.x = 0, .y = 0};
    new_node->entity.velocity = (Vector){.y = 0, .y = 0};

    new_node->next = entity_head;
    entity_head = new_node;

    return &(new_node->entity);
}
void world_set_cell_material(int cell_x, int cell_y, int m);
void world_set_cell_color(int cell_x, int cell_y, ALLEGRO_COLOR color);
void entity_update(Entity* entity)
{
    entity->position.x += entity->velocity.x;
    entity->position.y += entity->velocity.y;

    if(entity->use_bitmap)
    {
        int w = al_get_bitmap_width(entity->image.sprite);
        int h = al_get_bitmap_height(entity->image.sprite);
        ALLEGRO_COLOR color;

        int i, j;
        for(i = 0; i < w; i++)
        {
            for(j = 0; j < h; j++)
            {
                world_set_cell_material((int)(entity->position.x) + i,(int)(entity->position.y) + j, 12);
                color = al_get_pixel(entity->image.sprite, i, j);
                world_set_cell_color((int)(entity->position.x) + i,(int)(entity->position.y) + j, color);
            }
        }
    }
}


void entity_render(Entity* entity)
{
    
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