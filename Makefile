SRC_DIR = src
BUILD_DIR = build/debug
CC = clang
SRC_FILES = $(SRC_DIR)/BDEcoding.c
OBJ_NAME = app
# INCLUDE_PATHS = -I include/Raylib
# LIBRARY_PATHS = -L lib/Raylib
COMPILER_FLAGS = -Wall -Wextra -O0 -g
# LINKER_FLAGS = -l Raylib

all:
	$(CC) $(COMPILER_FLAGS) $(SRC_FILES) -o $(BUILD_DIR)/$(OBJ_NAME)
