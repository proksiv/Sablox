#include <stddef.h>
#include <stdio.h>
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

void entity_update(Entity* entity)
{
    entity->position.x += entity->velocity.x;
    entity->position.y += entity->velocity.y;
}

void entity_render(Entity* entity)
{
    printf("Pos: %f, %f\n", entity->position.x, entity->position.y);
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