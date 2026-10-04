CC = gcc
CFLAGS = -Wall -Wextra -std=c11
SRC_DIR = src
OBJ_DIR = obj
BIN = rebootc

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

TEST_BIN = test_combate
TEST_SRCS = tests/test_combate.c $(filter-out $(SRC_DIR)/main.c,$(SRCS))

.PHONY: all clean test

all: $(BIN)

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRCS) $(wildcard include/*.h)
	$(CC) $(CFLAGS) -o $@ $(TEST_SRCS)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BIN) $(TEST_BIN)
