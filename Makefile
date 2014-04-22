CC       := g++
LD       := g++
CXXFLAGS := -std=c++11 -Werror -Wall -O3
LDFLAGS  := -ljpeg

SRC_DIR := src
INCLUDE := include
BIN     := bin

OBJECTS := $(SRC_DIR)/sorting.o $(SRC_DIR)/sorting_context.o $(SRC_DIR)/read_write.o $(SRC_DIR)/main.o

all: mkbin bin/pixelsort

bin/pixelsort: $(OBJECTS)
	$(LD) -o $(@) $(CXXFLAGS) $(LDFLAGS) $(^)

mkbin:
	mkdir -p $(BIN)

clean: 
	rm -f $(OBJECTS)
	rm -rf $(BIN)
