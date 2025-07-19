#include "frontend.h"

#include <ncurses.h>

void draw_menu(int selected_item) {
  clear();

  attron(A_BOLD);
  mvprintw(5, 11, "T E T R I S");
  attroff(A_BOLD);

  char *menu_items[] = {"Start Game", "Controls", "Exit"};
  for (int i = 0; i < 3; i++) {
    if (i == selected_item) {
      attron(A_REVERSE | A_BOLD);
      mvprintw(10 + i * 2, 10, "> %s <", menu_items[i]);
      attroff(A_REVERSE | A_BOLD);
    } else {
      mvprintw(10 + i * 2, 10, "  %s  ", menu_items[i]);
    }
  }

  mvprintw(18, 10, "Use arrow keys to navigate, Enter to select");

  refresh();
}

void draw_game_field(int **field) {
  // clear();
  const int width = 10;
  const int height = 20;

  // Верхняя граница
  for (int x = 0; x < width + 1; x++) {
    mvprintw(0, x * 2, "--");
  }

  // Боковые границы и поле
  for (int y = 1; y <= height; y++) {
    mvprintw(y, 0, "|");
    for (int x = 0; x < width; x++) {
      if (field[y - 1][x]) {
        attron(COLOR_PAIR(field[y - 1][x]));
        mvprintw(y, x * 2 + 1, "[]");
        attroff(COLOR_PAIR(field[y - 1][x]));
      } else {
        mvprintw(y, x * 2 + 1, "  ");
      }
    }
    mvprintw(y, width * 2 + 1, "|");
  }

  // Нижняя граница
  for (int x = 0; x < width + 1; x++) {
    mvprintw(height + 1, x * 2, "--");
  }

  // Отрисовка текущей фигуры
  int piece_type = 0;
  for (int y = 0; y < 4 && !piece_type; y++) {
    for (int x = 0; x < 4 && !piece_type; x++) {
      if (game_state.current_piece[y][x])
        piece_type = game_state.current_piece[y][x];
    }
  }

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (game_state.current_piece[y][x]) {
        int screen_y = game_state.current_y + y + 1;
        int screen_x = (game_state.current_x + x) * 2 + 1;

        if (screen_y > 0 && screen_y <= height && screen_x > 0 &&
            screen_x <= width * 2) {
          attron(COLOR_PAIR(piece_type));
          mvprintw(screen_y, screen_x, "[]");
          attroff(COLOR_PAIR(piece_type));
        }
      }
    }
  }

  refresh();
}

void draw_side_panel(int **next, int score, int high_score, int level) {
  int start_x = 25;
  if (game_state.game_over) {
    clear();
    attron(A_BOLD | A_BLINK);
    mvprintw(10, 8, "G A M E   O V E R");
    attroff(A_BOLD | A_BLINK);

    mvprintw(12, 8, "Final Score: %d", game_state.score);
    mvprintw(14, 8, "Press 'M' for Menu");
    mvprintw(15, 8, "Press 'Q' to Quit");
    refresh();
    return;
  }

  mvprintw(2, start_x, "Next:");
  int piece_type = 0;
  for (int y = 0; y < 4 && !piece_type; y++) {
    for (int x = 0; x < 4 && !piece_type; x++) {
      if (next[y][x]) piece_type = next[y][x];
    }
  }

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (next[y][x]) {
        attron(COLOR_PAIR(piece_type));
        mvprintw(4 + y, start_x + x * 2, "[]");
        attroff(COLOR_PAIR(piece_type));
      } else {
        mvprintw(4 + y, start_x + x * 2, "  ");
      }
    }
  }
  if (game_state.pause) {
    attron(A_BOLD | A_BLINK);
    mvprintw(16, 25, "PAUSED");
    attroff(A_BOLD | A_BLINK);
  }

  mvprintw(10, start_x, "Score: %d", score);
  mvprintw(12, start_x, "High Score: %d", high_score);
  mvprintw(14, start_x, "Level: %d", level);

  refresh();
}

void init_colors() {
  start_color();
  init_pair(1, COLOR_YELLOW, COLOR_BLACK);   // O
  init_pair(2, COLOR_RED, COLOR_BLACK);      // Z
  init_pair(3, COLOR_GREEN, COLOR_BLACK);    // S
  init_pair(4, COLOR_CYAN, COLOR_BLACK);     // I
  init_pair(5, COLOR_WHITE, COLOR_BLACK);    // L
  init_pair(6, COLOR_BLUE, COLOR_BLACK);     // J
  init_pair(7, COLOR_MAGENTA, COLOR_BLACK);  // T
}