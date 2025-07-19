CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -g -D_POSIX_C_SOURCE=200809L
TEST_CFLAGS = -fprofile-arcs -ftest-coverage
LDFLAGS = -lncurses
TEST_LDFLAGS = -lcheck -lm -lpthread -lsubunit -lgcov --coverage

SRC_DIR = .
TETRIS_DIR = brick_game/tetris
GUI_DIR = gui/cli
TEST_DIR = tests
BUILD_DIR = build
BIN_DIR = bin

INSTALL_DIR = $(BIN_DIR)

SRCS = game.c $(TETRIS_DIR)/backend.c $(GUI_DIR)/frontend.c
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

TEST_SRCS = $(TEST_DIR)/test_backend.c
TEST_OBJS = $(BUILD_DIR)/$(TETRIS_DIR)/backend_gcov.o $(BUILD_DIR)/test_backend.o

TARGET = $(BIN_DIR)/tetris
TEST_TARGET = $(BIN_DIR)/tetris_test

.PHONY: all install uninstall clean dvi dist test gcov_report style

all: clean install run

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(TEST_TARGET): $(TEST_OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(TEST_CFLAGS) $^ -o $@ $(TEST_LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(TETRIS_DIR) -I$(GUI_DIR)

$(BUILD_DIR)/$(TETRIS_DIR)/backend_gcov.o: $(TETRIS_DIR)/backend.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(TEST_CFLAGS) -c $< -o $@ -I$(TETRIS_DIR) -I$(GUI_DIR)

$(BUILD_DIR)/test_backend.o: $(TEST_DIR)/test_backend.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(TETRIS_DIR) -I$(GUI_DIR)

install: $(TARGET)
	@mkdir -p $(INSTALL_DIR)
	@echo "Программа установлена в $(INSTALL_DIR)/tetris"
	@echo "Для запуска используйте: make run"

run:
	./bin/tetris

uninstall:
	@rm -rf $(BIN_DIR)
	@echo "$(BIN_DIR) было удалено"

clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR) *.gcda *.gcno *.gcov gcov_report coverage.info doc dist
	@echo "Очистка завершена"

test: $(TEST_TARGET)
	@./$(TEST_TARGET)

gcov_report: test
	@lcov --capture --directory $(BUILD_DIR)/$(TETRIS_DIR) --output-file coverage.info \
		--rc branch_coverage=1
	@genhtml coverage.info --output-directory gcov_report --branch-coverage
	@echo "Отчёт о покрытии: gcov_report/index.html"

dvi:
	@echo "Создание документации..."
	@mkdir -p doc
	@doxygen -g Doxyfile > /dev/null 2>&1 || true
	@echo "PROJECT_NAME = \"Tetris\"" >> Doxyfile
	@echo "INPUT = $(SRC_DIR) $(TETRIS_DIR) $(GUI_DIR)" >> Doxyfile
	@echo "RECURSIVE = YES" >> Doxyfile
	@echo "EXTRACT_ALL = YES" >> Doxyfile
	@echo "OUTPUT_DIRECTORY = doc" >> Doxyfile
	@doxygen Doxyfile
	@rm -f Doxyfile
	@echo "Документация создана в doc/"

dist: clean
	@echo "Создание дистрибутива..."
	@mkdir -p dist/tetris
	@rsync -a --exclude='dist/' --exclude='.git/' $(SRC_DIR)/ dist/tetris/ || \
		(echo "Если rsync не установлен, используйте cp"; \
		find $(SRC_DIR) -maxdepth 1 -mindepth 1 -not -name dist -not -name .git -exec cp -r {} dist/tetris/ \;)
	@find dist/tetris -name '*.gcda' -o -name '*.gcno' -o -name '*.gcov' -delete
	@tar -czvf dist/tetris.tar.gz -C dist tetris
	@rm -rf dist/tetris
	@echo "Дистрибутив создан: dist/tetris.tar.gz"

style:
	@echo "Проверка стиля кода..."
	@cp ../materials/linters/.clang-format .
	@clang-format -n game.c
	@clang-format -n $(TETRIS_DIR)/backend.c $(TETRIS_DIR)/backend.h
	@clang-format -n $(GUI_DIR)/frontend.c $(GUI_DIR)/frontend.h
	@rm .clang-format