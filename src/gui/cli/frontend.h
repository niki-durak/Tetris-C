#ifndef INTERFACE_H
#define INTERFACE_H

#include <ncurses.h>
#include <stdlib.h>

#include "../../brick_game/tetris/backend.h"

void draw_menu(int selected_item);
void draw_game_field(int **field);
void draw_side_panel(int **next, int score, int high_score, int level);
void init_colors();

#endif