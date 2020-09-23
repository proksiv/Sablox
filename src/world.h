#pragma once

#include "materials.h"

#define WORLD_W 162
#define WORLD_H 122

long steps;

void world_init();

bool world_get_updated(int cell_x, int cell_y);
void world_set_updated(int cell_x, int cell_y);

MATERIAL world_get_cell(int cell_x, int cell_y);
void world_set_cell(int cell_x, int cell_y, MATERIAL m);
bool world_move_cell(int src_x, int src_y, int dst_x, int dst_y);

void world_paint(int cell_x, int cell_y, MATERIAL m);

void world_update();
void world_render();