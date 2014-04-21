CC=g++
FLAGS=-std=c++11
LIBJPEG=-l jpeg
SRC=src
INCLUDE=include
BIN=bin

all: $(SRC)/sorting.o $(SRC)/sorting_context.o $(SRC)/read_write.o $(SRC)/main.o mkbin
	$(CC) $(LIBJPEG) $(FLAGS) $(SRC)/*.o -o bin/pixelsort

$(SRC)/main.o: $(SRC)/main.cpp
	$(CC) $(LIBJPEG) $(FLAGS) $(SRC)/main.cpp -c -o $(SRC)/main.o

$(SRC)/read_write.o: $(SRC)/read_write.cpp
	$(CC) $(LIBJPEG) $(FLAGS) $(SRC)/read_write.cpp -c -o $(SRC)/read_write.o

$(SRC)/sorting_context.o: $(SRC)/sorting_context.cpp
	$(CC) $(LIBJPEG) $(FLAGS) $(SRC)/sorting_context.cpp -c -o $(SRC)/sorting_context.o

$(SRC)/sorting.o: $(SRC)/sorting.cpp
	$(CC) $(LIBJPEG) $(FLAGS) $(SRC)/sorting.cpp -c -o $(SRC)/sorting.o

mkbin:
	mkdir -p $(BIN)

clean: 
	rm -f *.o $(SRC)/*.o
	rm -rf $(BIN)
