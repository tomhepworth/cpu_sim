COMPILER := g++
CFLAGS = -g -Wall -std=c++11
SOURCE_DIR := src
BUILD_DIR := build
TEST_LIBS := src/libs/catch/
VPATH = src/ src/parser/ src/isa/ src/cpu/ src/tests/ src/debug/ src/tomasulos

#cppclean --include-path=src/ --include-path=src/parser/ --include-path=src/isa/ --include-path=src/cpu/ --include-path=src/tests/ --include-path=src/debug/ .

CPP_SRCS := $(shell find $(SOURCE_DIR)/ -name \*.cpp ! -name "main.cpp" ! -path "$(SOURCE_DIR)/tests/*" -print)
CPP_TESTS := $(shell find $(SOURCE_DIR)/tests/ -name \*.cpp -print)
CPP_OBJS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(CPP_SRCS)))
CPP_TEST_OBJS := $(patsubst %.cpp, %.o, $(notdir $(CPP_TESTS)))

HEADER_INCLUDES = $(addprefix -I, $(VPATH))

.PHONY: all sim test run simulator tests dir clean

all: dir simulator test
sim: dir simulator
test: dir testscalar testooo

run: all
	./test
	./simulator

# Set build dir and build simulator
simulator: $(CPP_OBJS) $(BUILD_DIR)/main.o
	$(COMPILER) $(CFLAGS) -o $@ $^


# For building tests include testing library (catch2), add flags for test mode, and set build output directory
testscalar: HEADER_INCLUDES += -Isrc/libs/catch/
testscalar: $(CPP_OBJS) $(BUILD_DIR)/test_scalar.o
	$(COMPILER) $(CFLAGS) -o scalartest $^ $(HEADER_INCLUDES)

testooo: HEADER_INCLUDES += -Isrc/libs/catch/
testooo: $(CPP_OBJS) $(BUILD_DIR)/test_ooo.o
	$(COMPILER) $(CFLAGS) -o oootest $^ $(HEADER_INCLUDES)



# Build object files from cpp files
$(BUILD_DIR)/%.o: %.cpp
	$(COMPILER) -c $(CFLAGS) $< -o $@ $(HEADER_INCLUDES)

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
	rm test

# -include $(CPP_OBJS:.o=.d)