
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/artifacts
INCLUDE_DIR = ./include

SRCS = $(notdir $(wildcard $(SRC_DIR)/*.c))
OBJS = $(addsuffix .o, $(basename $(SRCS)))

CFLAGS ?= -Werror -Wall

build: build_dir $(OBJS)
	$(CC) -shared -o $(BUILD_DIR)/libopensmarts.so $(addprefix $(OBJ_DIR)/, $(OBJS))

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -fpic -I$(INCLUDE_DIR) -o $(BUILD_DIR)/artifacts/$@ $<

build_dir:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)

