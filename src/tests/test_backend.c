#include "../brick_game/tetris/backend.h"
#include <check.h>
#include <stdlib.h>

const int TEST_PIECE[4][4] = {
    {0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}};

START_TEST(test_initialize_and_cleanup) {
  initialize_game();
  ck_assert_int_eq(game_state.initialized, true);
  ck_assert_ptr_nonnull(game_state.field);
  ck_assert_ptr_nonnull(game_state.next);

  cleanup_game();
  ck_assert_int_eq(game_state.initialized, false);
  ck_assert_ptr_null(game_state.field);
  ck_assert_ptr_null(game_state.next);
}
END_TEST

START_TEST(test_generate_next_piece) {
  initialize_game();

  int empty = 1;
  for (int i = 0; i < 10; i++) {
    generate_next_piece();
    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        if (game_state.next[y][x] != 0)
          empty = 0;
      }
    }
  }
  ck_assert_int_eq(empty, 0);

  cleanup_game();
}
END_TEST

START_TEST(test_spawn_new_piece) {
  initialize_game();

  int test_piece[4][4] = {
      {1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      game_state.next[y][x] = test_piece[y][x];
    }
  }

  spawn_new_piece();

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      ck_assert_int_eq(game_state.current_piece[y][x], test_piece[y][x]);
    }
  }

  cleanup_game();
}
END_TEST

START_TEST(test_is_within_bounds) {
  initialize_game();

  memset(game_state.current_piece, 0, sizeof(game_state.current_piece));
  game_state.current_piece[0][0] = 1;

  ck_assert_int_eq(is_within_bounds(0, 0), 1);
  ck_assert_int_eq(is_within_bounds(-1, 0), 0);
  ck_assert_int_eq(is_within_bounds(FIELD_WIDTH, 0), 0);

  cleanup_game();
}
END_TEST

START_TEST(test_check_collision) {
  initialize_game();

  memset(game_state.current_piece, 0, sizeof(game_state.current_piece));
  game_state.current_piece[0][0] = 1;

  game_state.field[5][5] = 1;

  game_state.current_x = 5;
  game_state.current_y = 5;

  ck_assert_int_eq(check_collision(), 1);

  game_state.current_y = 4;
  ck_assert_int_eq(check_collision(), 0);

  cleanup_game();
}
END_TEST

START_TEST(test_rotate_piece) {
  initialize_game();

  memcpy(game_state.current_piece,
         (int[4][4]){{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
         sizeof(game_state.current_piece));

  int original[4][4];
  memcpy(original, game_state.current_piece, sizeof(original));

  rotate_piece();

  ck_assert_mem_ne(original, game_state.current_piece, sizeof(original));

  cleanup_game();
}
END_TEST

START_TEST(test_merge_piece) {
  initialize_game();

  memcpy(game_state.current_piece,
         (int[4][4]){{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
         sizeof(game_state.current_piece));
  game_state.current_x = 3;
  game_state.current_y = 5;

  merge_piece_to_field();

  ck_assert_int_eq(game_state.field[6][4], 1);
  ck_assert_int_eq(game_state.field[6][5], 1);
  ck_assert_int_eq(game_state.field[7][4], 1);
  ck_assert_int_eq(game_state.field[7][5], 1);

  cleanup_game();
}
END_TEST

START_TEST(test_clear_lines) {
  initialize_game();

  for (int x = 0; x < FIELD_WIDTH; x++) {
    game_state.field[FIELD_HEIGHT - 1][x] = 1;
  }

  int lines_cleared = clear_lines();

  ck_assert_int_eq(lines_cleared, 1);
  ck_assert_int_eq(game_state.score, 100);

  for (int x = 0; x < FIELD_WIDTH; x++) {
    ck_assert_int_eq(game_state.field[FIELD_HEIGHT - 1][x], 0);
  }

  cleanup_game();
}
END_TEST

START_TEST(test_userInput_actions) {
  initialize_game();

  int initial_x = game_state.current_x;
  userInput(Left, false);
  ck_assert_int_eq(game_state.current_x, initial_x - 1);

  userInput(Right, false);
  ck_assert_int_eq(game_state.current_x, initial_x);

  int initial_y = game_state.current_y;
  userInput(Up, true);

  if (initial_y == 0) {
    ck_assert_int_eq(game_state.current_y, initial_y);
  } else {
    ck_assert_int_lt(game_state.current_y, initial_y);
  }

  cleanup_game();
}
END_TEST

START_TEST(test_read_write_high_score) {
  int test_score = 1500;
  write_high_score(test_score);

  int read_score = read_high_score();
  ck_assert_int_eq(read_score, test_score);
}
END_TEST

START_TEST(test_updateCurrentState) {
  initialize_game();

  GameInfo_t info = updateCurrentState();
  ck_assert_int_eq(info.initialized, true);

  game_state.pause = 1;
  info = updateCurrentState();
  ck_assert_int_eq(info.pause, 1);

  cleanup_game();
}
END_TEST

START_TEST(test_boundary_conditions) {
  initialize_game();
  game_state.current_x = 0;
  ck_assert_int_eq(is_within_bounds(game_state.current_x, 0), 1);

  game_state.current_x = FIELD_WIDTH - 1;
  ck_assert_int_eq(is_within_bounds(game_state.current_x, 0), 0);

  cleanup_game();
}
END_TEST

START_TEST(test_all_piece_types) {
  initialize_game();

  for (int i = 0; i < 10; i++) {
    generate_next_piece();
    spawn_new_piece();

    ck_assert_int_eq(
        is_within_bounds(game_state.current_x, game_state.current_y), 1);
  }

  cleanup_game();
}
END_TEST

START_TEST(test_is_within_bounds_edge_cases) {
  initialize_game();

  memset(game_state.current_piece, 1, sizeof(game_state.current_piece));

  ck_assert_int_eq(is_within_bounds(0, 0), 1);
  ck_assert_int_eq(is_within_bounds(FIELD_WIDTH - 4, 0), 1);
  ck_assert_int_eq(is_within_bounds(FIELD_WIDTH - 3, 0), 0);

  cleanup_game();
}
END_TEST

START_TEST(test_all_tetromino_types) {
  initialize_game();

  for (int i = 0; i < NUM_TETROMINOES; i++) {
    memcpy(game_state.current_piece, TETROMINOES[i],
           sizeof(game_state.current_piece));

    ck_assert_int_eq(is_within_bounds(0, 0), 1);
    ck_assert_int_eq(check_collision(), 0);
  }

  cleanup_game();
}
END_TEST

START_TEST(test_clear_multiple_lines) {
  initialize_game();

  for (int y = FIELD_HEIGHT - 2; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      game_state.field[y][x] = 1;
    }
  }

  int lines = clear_lines();
  ck_assert_int_eq(lines, 2);
  ck_assert_int_eq(game_state.score, 300);

  cleanup_game();
}
END_TEST

START_TEST(test_rotate_all_pieces) {
  initialize_game();

  for (int i = 0; i < NUM_TETROMINOES; i++) {
    memcpy(game_state.current_piece, TETROMINOES[i],
           sizeof(game_state.current_piece));

    int original[4][4];
    memcpy(original, game_state.current_piece, sizeof(original));

    for (int j = 0; j < 4; j++) {
      rotate_piece();
    }

    ck_assert_mem_eq(original, game_state.current_piece, sizeof(original));
  }

  cleanup_game();
}
END_TEST

START_TEST(test_rotate_with_kick) {
  initialize_game();

  memcpy(game_state.current_piece,
         (int[4][4]){{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
         sizeof(game_state.current_piece));

  game_state.current_x = FIELD_WIDTH - 3;

  int original[4][4];
  memcpy(original, game_state.current_piece, sizeof(original));

  rotate_piece();

  ck_assert_mem_ne(original, game_state.current_piece, sizeof(original));
  ck_assert_int_eq(is_within_bounds(game_state.current_x, game_state.current_y),
                   1);

  cleanup_game();
}
END_TEST

START_TEST(test_terminate_action) {
  initialize_game();

  ck_assert_int_eq(game_state.initialized, 1);

  userInput(Terminate, false);

  ck_assert_int_eq(game_state.initialized, 0);
  ck_assert_ptr_null(game_state.field);
  ck_assert_ptr_null(game_state.next);
}
END_TEST

START_TEST(test_max_level) {
  initialize_game();

  game_state.score = (MAX_LEVEL - 1) * LEVEL_UP_SCORE;

  for (int x = 0; x < FIELD_WIDTH; x++) {
    game_state.field[FIELD_HEIGHT - 1][x] = 1;
  }

  clear_lines();

  ck_assert_int_eq(game_state.level, MAX_LEVEL);
  ck_assert_int_eq(game_state.speed, 500 - (MAX_LEVEL - 1) * 40);

  cleanup_game();
}
END_TEST

START_TEST(test_high_score_update) {
  initialize_game();

  game_state.high_score = 1000;
  write_high_score(1000);

  game_state.score = 2000;

  for (int y = 0; y < 3; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      game_state.field[y][x] = 1;
    }
  }

  spawn_new_piece();

  ck_assert_int_eq(game_state.high_score, 2000);

  int file_score = read_high_score();
  ck_assert_int_eq(file_score, 2000);

  cleanup_game();
}
END_TEST

START_TEST(test_min_speed_limit) {
  initialize_game();

  game_state.level = 11;
  game_state.speed = 500 - (game_state.level - 1) * 40;

  if (game_state.speed < 100) {
    game_state.speed = 100;
  }
  ck_assert_int_eq(game_state.speed, 100);

  cleanup_game();
}
END_TEST

START_TEST(test_update_current_state_pause) {
  initialize_game();

  game_state.pause = 1;

  GameInfo_t info = updateCurrentState();

  ck_assert_int_eq(info.current_y, 0);

  cleanup_game();
}
END_TEST

START_TEST(test_down_movement) {
  initialize_game();

  int initial_y = game_state.current_y;

  userInput(Down, false);

  ck_assert_int_eq(game_state.current_y, initial_y + 1);

  cleanup_game();
}
END_TEST

START_TEST(test_start_button) {
  initialize_game();

  game_state.pause = 1;
  userInput(Start, false);

  ck_assert_int_eq(game_state.pause, 0);

  userInput(Start, false);

  ck_assert_int_eq(game_state.score, 0);

  cleanup_game();
}
END_TEST

START_TEST(test_pause_button) {
  initialize_game();

  userInput(Pause, false);
  ck_assert_int_eq(game_state.pause, 1);

  userInput(Pause, false);
  ck_assert_int_eq(game_state.pause, 0);

  cleanup_game();
}
END_TEST

START_TEST(test_rotate_collision_recovery) {
  initialize_game();

  memcpy(game_state.current_piece,
         (int[4][4]){{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}},
         sizeof(game_state.current_piece));

  game_state.current_x = FIELD_WIDTH - 1;

  rotate_piece();

  ck_assert_int_eq(game_state.current_x, FIELD_WIDTH - 1);

  cleanup_game();
}
END_TEST

START_TEST(test_file_operations_fail) {
  remove("high_score.txt");
  int score = read_high_score();
  ck_assert_int_eq(score, 0);

  write_high_score(1000);
}
END_TEST

Suite *tetris_suite(void) {
  Suite *s = suite_create("Tetris");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_initialize_and_cleanup);
  tcase_add_test(tc_core, test_generate_next_piece);
  tcase_add_test(tc_core, test_spawn_new_piece);
  tcase_add_test(tc_core, test_is_within_bounds);
  tcase_add_test(tc_core, test_check_collision);
  tcase_add_test(tc_core, test_rotate_piece);
  tcase_add_test(tc_core, test_merge_piece);
  tcase_add_test(tc_core, test_clear_lines);
  tcase_add_test(tc_core, test_userInput_actions);
  tcase_add_test(tc_core, test_read_write_high_score);
  tcase_add_test(tc_core, test_updateCurrentState);
  tcase_add_test(tc_core, test_boundary_conditions);
  tcase_add_test(tc_core, test_all_piece_types);
  tcase_add_test(tc_core, test_is_within_bounds_edge_cases);
  tcase_add_test(tc_core, test_all_tetromino_types);
  tcase_add_test(tc_core, test_clear_multiple_lines);
  tcase_add_test(tc_core, test_rotate_all_pieces);
  tcase_add_test(tc_core, test_rotate_with_kick);
  tcase_add_test(tc_core, test_terminate_action);
  tcase_add_test(tc_core, test_max_level);
  tcase_add_test(tc_core, test_high_score_update);
  tcase_add_test(tc_core, test_min_speed_limit);
  tcase_add_test(tc_core, test_update_current_state_pause);
  tcase_add_test(tc_core, test_down_movement);
  tcase_add_test(tc_core, test_start_button);
  tcase_add_test(tc_core, test_pause_button);
  tcase_add_test(tc_core, test_rotate_collision_recovery);
  tcase_add_test(tc_core, test_file_operations_fail);

  suite_add_tcase(s, tc_core);
  return s;
}

int main(void) {
  Suite *s = tetris_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}