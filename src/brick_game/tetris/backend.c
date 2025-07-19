#include "backend.h"

GameInfo_t game_state = {0};

const int TETROMINOES[NUM_TETROMINOES][4][4] = {
    // I
    {{4, 4, 4, 4}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    // J
    {{6, 0, 0, 0}, {6, 6, 6, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    // L
    {{0, 0, 5, 0}, {5, 5, 5, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    // O
    {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    // S
    {{0, 3, 3, 0}, {3, 3, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    // T
    {{0, 7, 0, 0}, {7, 7, 7, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    // Z
    {{2, 2, 0, 0}, {0, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}};

void initialize_game() {
  if (game_state.initialized) {
    cleanup_game();
  }
  game_state.high_score = read_high_score();

  // Инициализация случайных чисел
  srand(time(NULL));
  game_state.game_over = false;

  // Игровое поле
  game_state.field = (int **)malloc(FIELD_HEIGHT * sizeof(int *));
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    game_state.field[i] = (int *)calloc(FIELD_WIDTH, sizeof(int));
  }

  // Фигура "next"
  game_state.next = (int **)malloc(4 * sizeof(int *));
  for (int i = 0; i < 4; i++) {
    game_state.next[i] = (int *)calloc(4, sizeof(int));
  }

  game_state.score = 0;
  game_state.level = 1;
  game_state.speed = 500;
  game_state.pause = 0;
  game_state.initialized = true;

  generate_next_piece();
  spawn_new_piece();
  generate_next_piece();
}

void cleanup_game() {
  if (game_state.field) {
    for (int i = 0; i < FIELD_HEIGHT; i++) {
      if (game_state.field[i]) free(game_state.field[i]);
    }
    free(game_state.field);
    game_state.field = NULL;
  }

  if (game_state.next) {
    for (int i = 0; i < 4; i++) {
      if (game_state.next[i]) free(game_state.next[i]);
    }
    free(game_state.next);
    game_state.next = NULL;
  }

  game_state.initialized = false;
}

void generate_next_piece() {
  int piece_type = rand() % NUM_TETROMINOES;
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      game_state.next[y][x] = TETROMINOES[piece_type][y][x];
    }
  }
  if (check_collision()) {
    game_state.game_over = true;
  }
}

void spawn_new_piece() {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      game_state.current_piece[y][x] = game_state.next[y][x];
    }
  }

  game_state.current_x = FIELD_WIDTH / 2 - 2;
  game_state.current_y = 0;

  if (check_collision()) {
    game_state.game_over = true;
    if (game_state.score > game_state.high_score) {
      game_state.high_score = game_state.score;
      write_high_score(game_state.high_score);
    }
  }
}

int check_collision() {
  return !is_within_bounds(game_state.current_x, game_state.current_y);
}

void rotate_piece() {
  int temp[4][4];

  // Сохраняем текущее состояние фигуры и позиции
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      temp[y][x] = game_state.current_piece[y][x];
    }
  }

  // Поворачиваем фигуру (90 градусов по часовой стрелке)
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      game_state.current_piece[x][3 - y] = temp[y][x];
    }
  }

  // Если после поворота есть коллизия, пробуем сдвинуть фигуру
  if (check_collision()) {
    int original_x = game_state.current_x;

    int kick_attempts = 0;
    int kick_offsets[] = {0, -1, 1, -2, 2};  // Последовательность сдвигов

    while (kick_attempts < 5 && check_collision()) {
      game_state.current_x = original_x + kick_offsets[kick_attempts];
      kick_attempts++;
    }

    if (check_collision()) {
      game_state.current_x = original_x;
      for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
          game_state.current_piece[y][x] = temp[y][x];
        }
      }
    }
  }
}

int is_within_bounds(int x, int y) {
  for (int py = 0; py < 4; py++) {
    for (int px = 0; px < 4; px++) {
      if (game_state.current_piece[py][px]) {
        int field_x = x + px;
        int field_y = y + py;
        if (field_x < 0 || field_x >= FIELD_WIDTH || field_y >= FIELD_HEIGHT) {
          return 0;
        }
        if (field_y >= 0 && game_state.field[field_y][field_x]) {
          return 0;
        }
      }
    }
  }
  return 1;
}

void merge_piece_to_field() {
  // Определяем тип текущей фигуры
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
        int field_y = game_state.current_y + y;
        int field_x = game_state.current_x + x;
        if (field_y >= 0 && field_y < FIELD_HEIGHT && field_x >= 0 &&
            field_x < FIELD_WIDTH) {
          game_state.field[field_y][field_x] =
              piece_type;  // Сохраняем тип фигуры
        }
      }
    }
  }
}

int clear_lines() {
  int lines_cleared = 0;
  int scores_table[] = {0, 100, 300, 700, 1500};

  for (int y = FIELD_HEIGHT - 1; y >= 0; y--) {
    int line_full = 1;
    for (int x = 0; x < FIELD_WIDTH; x++) {
      if (!game_state.field[y][x]) {
        line_full = 0;
        break;
      }
    }

    if (line_full) {
      // Удаляем линию и сдвигаем все вышестоящие линии вниз
      for (int yy = y; yy > 0; yy--) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
          game_state.field[yy][x] = game_state.field[yy - 1][x];
        }
      }

      // Очищаем верхнюю линию
      for (int x = 0; x < FIELD_WIDTH; x++) {
        game_state.field[0][x] = 0;
      }

      lines_cleared++;
      y++;  // Проверяем текущую строку снова
    }
  }

  // Начисляем очки по таблице
  if (lines_cleared > 0 && lines_cleared < 5) {
    game_state.score += scores_table[lines_cleared];

    // Проверяем повышение уровня (каждые 600 очков)
    int new_level = game_state.score / LEVEL_UP_SCORE + 1;
    if (new_level > game_state.level && new_level <= MAX_LEVEL) {
      game_state.level = new_level;
      // Увеличиваем скорость (базовая 500 мс, уменьшаем на 40 мс за уровень)
      game_state.speed = 500 - (game_state.level - 1) * 40;
      if (game_state.speed < 100)
        game_state.speed = 100;  // Минимальная скорость
    }
  }

  return lines_cleared;
}

void userInput(UserAction_t action, bool hold) {
  if (game_state.game_over || !game_state.initialized) return;

  switch (action) {
    case Left:
      game_state.current_x--;
      if (check_collision()) game_state.current_x++;
      break;

    case Right:
      game_state.current_x++;
      if (check_collision()) game_state.current_x--;
      break;

    case Down:
      game_state.current_y++;
      if (check_collision()) {
        game_state.current_y--;
        merge_piece_to_field();
        clear_lines();
        spawn_new_piece();
        generate_next_piece();
      }
      break;

    case Action:
      rotate_piece();
      break;

    case Up:
      if (hold) {
        while (!check_collision()) {
          game_state.current_y++;
        }
        game_state.current_y--;
        merge_piece_to_field();
        clear_lines();
        spawn_new_piece();
        generate_next_piece();
      } else {
        game_state.current_y++;
        if (check_collision()) {
          game_state.current_y--;
          merge_piece_to_field();
          clear_lines();
          spawn_new_piece();
          generate_next_piece();
        }
      }
      break;

    case Start:
      if (game_state.pause) {
        game_state.pause = 0;
      } else {
        initialize_game();
      }
      break;

    case Pause:
      game_state.pause = !game_state.pause;
      break;
    case Terminate:
      cleanup_game();
      break;
  }
}

static clock_t last_drop_time = 0;

int read_high_score() {
  FILE *file = fopen("high_score.txt", "r");
  if (file == NULL) {
    return 0;
  }

  int high_score = 0;
  fscanf(file, "%d", &high_score);
  fclose(file);

  return high_score;
}

void write_high_score(int score) {
  FILE *file = fopen("high_score.txt", "w");
  if (file == NULL) {
    return;
  }

  fprintf(file, "%d", score);
  fclose(file);
}

GameInfo_t updateCurrentState() {
  clock_t current_time = clock();
  if (game_state.game_over || game_state.pause || !game_state.initialized) {
    return game_state;
  }

  if (!game_state.pause && game_state.initialized) {
    if ((current_time - last_drop_time) * 1000 / CLOCKS_PER_SEC >
        game_state.speed) {
      last_drop_time = current_time;
      userInput(Down, false);
    }
  }

  return game_state;
}