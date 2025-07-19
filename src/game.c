#include <ncurses.h>
#include <unistd.h>

#include "brick_game/tetris/backend.h"
#include "gui/cli/frontend.h"

int main() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  if (has_colors()) {
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);   // O
    init_pair(2, COLOR_RED, COLOR_BLACK);      // Z
    init_pair(3, COLOR_GREEN, COLOR_BLACK);    // S
    init_pair(4, COLOR_CYAN, COLOR_BLACK);     // I
    init_pair(5, COLOR_WHITE, COLOR_BLACK);    // L
    init_pair(6, COLOR_BLUE, COLOR_BLACK);     // J
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);  // T
  }

  initialize_game();
  if (!game_state.initialized) {
    endwin();
    fprintf(stderr, "Failed to initialize game!\n");
    return 1;
  }

  int in_menu = 1;  // 1 — в меню, 0 — в игре
  int selected_item = 0;

  while (1) {
    if (in_menu) {
      draw_menu(selected_item);
      nodelay(stdscr, FALSE);
      int ch = getch();
      switch (ch) {
        case KEY_UP:
          if (selected_item > 0) selected_item--;
          break;
        case KEY_DOWN:
          if (selected_item < 2) selected_item++;
          break;
        case 10:
          if (selected_item == 0) {
            clear();
            in_menu = 0;
            userInput(Start, false);
          } else if (selected_item == 1) {
            clear();
            mvprintw(5, 10, "Controls:");
            mvprintw(7, 10, "Left/Right arrows - Move piece");
            mvprintw(8, 10, "Up arrow - Fast drop");
            mvprintw(9, 10, "Down arrow - Speed up falling");
            mvprintw(10, 10, "Space - Rotate piece");
            mvprintw(11, 10, "P - Pause game");
            mvprintw(12, 10, "M - Return to menu");
            mvprintw(13, 10, "Q - Quit game");
            mvprintw(15, 10, "Press any key to return...");
            refresh();
            getch();
          } else if (selected_item == 2) {
            endwin();
            return 0;
          }
          break;
        case 'q':
          endwin();
          return 0;
      }
    } else {
      if (game_state.game_over) {
        nodelay(stdscr, FALSE);
        int ch = getch();
        if (ch == 'm') {
          in_menu = 1;
          initialize_game();
        } else if (ch == 'q') {
          endwin();
          return 0;
        }
        continue;
      }
      nodelay(stdscr, TRUE);
      int ch = getch();
      if (ch != ERR) {
        switch (ch) {
          case 'q':
            if (game_state.score > game_state.high_score) {
              write_high_score(game_state.score);
            }
            userInput(Terminate, false);
            endwin();
            return 0;
          case KEY_LEFT:
            userInput(Left, false);
            break;
          case KEY_RIGHT:
            userInput(Right, false);
            break;
          case KEY_UP:
            userInput(Up, true);
            break;
          case KEY_DOWN:
            userInput(Down, false);
            break;
          case ' ':
            userInput(Action, false);
            break;
          case 'p':
            clear();
            userInput(Pause, false);
            break;
          case 'm':
            if (game_state.score > game_state.high_score) {
              game_state.high_score = game_state.score;
              write_high_score(game_state.high_score);
            }
            in_menu = 1;
            break;
        }
      }

      GameInfo_t game_info = updateCurrentState();
      draw_game_field(game_info.field);
      draw_side_panel(game_info.next, game_info.score, game_info.high_score,
                      game_info.level);

      // napms(50);
    }
  }
  cleanup_game();
  endwin();
  return 0;
}