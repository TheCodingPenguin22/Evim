# Name of the final executable
TARGET = bin/evim

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

# Directories
SRC_DIR = src
BIN_DIR = bin

# Source and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)

# Default target
all: $(TARGET)

# Link the object files
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile each .c file into .o inside bin/
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Include the dependency files
-include $(DEPS)

# Clean up
clean:
	rm -rf $(BIN_DIR)/*.o $(DEPS) $(TARGET)
