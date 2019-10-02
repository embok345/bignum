ROOT_PATH = $(abspath $(lastword $(MAKEFILE_LIST)))
SRC_DIR = $(dir $(ROOT_PATH))src
BUILD_DIR = $(dir $(ROOT_PATH))bin

CC = gcc

ifeq ($(RELEASE),1)
  COMPILE_FLAGS = -O3 -w
endif
ifeq ($(DEBUG),1)
  COMPILE_FLAGS = -Og -g -Wall
endif

CFLAGS = $(COMPILE_FLAGS)
LDFLAGS = -lm

SRCS = $(wildcard $(SRC_DIR)/*.c)
STATIC_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/static/%.o,$(SRCS))
SHARED_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/shared/%.o,$(SRCS))

DEPS = $(wildcard $(SRC_DIR)/*.h)

static: $(STATIC_OBJS)
	ar rcs $(BUILD_DIR)/static/libbignum.a $(STATIC_OBJS)
$(STATIC_OBJS): $(BUILD_DIR)/static/%.o : $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c $< -o $@ $(CFLAGS)

shared: $(SHARED_OBJS)
	$(CC) -shared -o $(BUILD_DIR)/shared/libbignum.so $(SHARED_OBJS) $(CFLAGS) $(LD_FLAGS)
$(SHARED_OBJS): $(BUILD_DIR)/shared/%.o : $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c $< -o $@ $(CFLAGS) -fPIC

.PHONY: clean
clean:
	rm -f $(BUILD_DIR)/shared/*.o $(BUILD_DIR)/static/*.o
