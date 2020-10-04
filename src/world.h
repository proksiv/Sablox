#pragma once

#include "materials.h"

#define WORLD_W 386
#define WORLD_H 218

extern long steps;

void world_init();

bool world_get_cell_updated(int cell_x, int cell_y);
void world_set_cell_updated(int cell_x, int cell_y);
MATERIAL world_get_cell_material(int cell_x, int cell_y);
void world_set_cell_material(int cell_x, int cell_y, MATERIAL m);
lifetime_t world_get_cell_lifetime(int cell_x, int cell_y);
void world_set_cell_lifetime(int cell_x, int cell_y, lifetime_t lifetime);

bool world_move_cell(int src_x, int src_y, int dst_x, int dst_y);
void world_swap_cells(int ax, int ay, int bx, int by);
void world_paint(int cell_x, int cell_y, MATERIAL m);

void world_update();
void world_render();