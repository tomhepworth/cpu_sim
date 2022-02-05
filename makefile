COMPILER := g++
CFLAGS = -g -Wall
SOURCE_DIR := src
INCLUDES := $(SUBDIR_INCLUDES)
BUILD_DIR := build
VPATH = src src/parser/ src/isa/

# CPP_SRCS := $(shell find $(SOURCE_DIR) -name \*.h -print -o -name \*.cpp -print)
CPP_SRCS := $(shell find $(SOURCE_DIR)/ -name \*.cpp -print)
# CPP_HEADERS := $(shell find $(SOURCE_DIR)/ -name \*.h -print)
CPP_OBJS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(CPP_SRCS)))

HEADER_INCLUDES = $(addprefix -I, $(VPATH))

all: dir simulator

run: all
	./simulator

simulator: $(CPP_OBJS)
	$(COMPILER) $(CFLAGS) -o $@ $^ 

$(BUILD_DIR)/%.o: %.cpp
	@echo $@
	$(COMPILER) -c $(CFLAGS) $< -o $@ $(HEADER_INCLUDES)

dir:	
	@echo $(CPP_SRCS)
	@echo $(CPP_OBJS)
	mkdir -p build

clean:
	rm -rf $(BUILD_DIR)
	rm simulator

# -include $(CPP_OBJS:.o=.d)