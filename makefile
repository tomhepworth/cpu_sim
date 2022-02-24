COMPILER := g++
CFLAGS = -g -Wall -std=c++11
SOURCE_DIR := src
BUILD_DIR := build
TEST_LIBS := src/libs/catch/
VPATH = src/ src/parser/ src/isa/ src/cpu/ src/tests/ src/debug/

#cppclean --include-path=src/ --include-path=src/parser/ --include-path=src/isa/ --include-path=src/cpu/ --include-path=src/tests/ --include-path=src/debug/ .

CPP_SRCS := $(shell find $(SOURCE_DIR)/ -name \*.cpp ! -name "main.cpp" ! -path "$(SOURCE_DIR)/tests/*" -print)
CPP_TESTS := $(shell find $(SOURCE_DIR)/tests/ -name \*.cpp -print)
CPP_OBJS := $(patsubst %.cpp, %.o, $(notdir $(CPP_SRCS)))
CPP_TEST_OBJS := $(patsubst %.cpp, %.o, $(notdir $(CPP_TESTS)))

HEADER_INCLUDES = $(addprefix -I, $(VPATH))

.PHONY: all sim test run simulator tests dir clean

all: dir simulator test
sim: dir simulator
test: dir tests

run: all
	./test
	./simulator

# Set build dir and build simulator
simulator: BUILD_DIR = build/sim
simulator: $(CPP_OBJS) main.o
	$(COMPILER) $(CFLAGS) -o $@ $(addprefix $(BUILD_DIR)/, $^)


# For building tests include testing library (catch2), add flags for test mode, and set build output directory
tests: HEADER_INCLUDES += -Isrc/libs/catch/
tests: CFLAGS += -DTEST_MODE
tests: BUILD_DIR = build/test
tests: $(CPP_OBJS) $(CPP_TEST_OBJS)
	$(COMPILER) $(CFLAGS) -o test $(addprefix $(BUILD_DIR)/, $^) -I$(TEST_LIBS)


# Build object files from cpp files
%.o: %.cpp
	$(COMPILER) -c $(CFLAGS) $< -o $(BUILD_DIR)/$@ $(HEADER_INCLUDES)

# Make the output directories
dir:	
	@echo $(CPP_SRCS)
	@echo $(CPP_OBJS)
	mkdir -p build
	mkdir -p build/sim
	mkdir -p build/test

clean:
	rm -rf $(BUILD_DIR)
	rm simulator

# -include $(CPP_OBJS:.o=.d)