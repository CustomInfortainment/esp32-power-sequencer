CC = gcc
CFLAGS = -Iinclude -MMD -MP -g

BUILD_DIR = build
SRC_DIR = src
INC_DIR = include

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

TARGET = $(BUILD_DIR)/main.out

$(TARGET): $(OBJS)
	$(CC) -g -o $@ $^ -lncurses

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@  $<

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR)