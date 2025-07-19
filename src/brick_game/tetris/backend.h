#ifndef BACKEND_H
#define BACKEND_H

#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 20
#define NUM_TETROMINOES 7
#define MAX_LEVEL 10
#define LEVEL_UP_SCORE 600

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
  bool initialized;
  int current_piece[4][4];
  int next_piece[4][4];
  int current_x;
  int current_y;
  bool game_over;
} GameInfo_t;

extern GameInfo_t game_state;

extern const int TETROMINOES[NUM_TETROMINOES][4][4];

void userInput(UserAction_t action, bool hold);
void initialize_game();
void cleanup_game();
GameInfo_t updateCurrentState();
void spawn_new_piece();
void rotate_piece();
int check_collision();
int is_within_bounds(int x, int y);
void merge_piece_to_field();
int clear_lines();
void generate_next_piece();
int read_high_score();
void write_high_score(int score);

#endif