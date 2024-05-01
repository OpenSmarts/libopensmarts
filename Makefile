
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

SRCS = $(wildcard $SRC_DIR/*.c)
OBJS = $(addprefix $(BUILD_DIR)/artifacts, $(addsuffix .o, $(basename $(SRCS))))

build: $(OBJS)
	gcc -shared -o $(BUILD_DIR)/libopensmarts.so $(OBJS)
