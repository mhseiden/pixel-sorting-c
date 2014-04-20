CC=g++
FLAGS=-std=c++11
LIBJPEG=-l jpeg
SRC=src
INCLUDE=include
BIN=bin

all: $(SRC)/read_write.o $(SRC)/sorters.o mkbin
	$(CC) $(LIBJPEG) $(FLAGS) $(SRC)/main.cpp -c -o $(SRC)/main.o
	$(CC) $(LIBJPEG) $(FLAGS) $(SRC)/*.o -o bin/pixelsort

$(SRC)/read_write.o: $(SRC)/read_write.cpp
	$(CC) $(LIBJPEG) $(FLAGS) $(SRC)/read_write.cpp -c -o $(SRC)/read_write.o

$(SRC)/sorters.o: $(SRC)/sorters.cpp
	$(CC) $(LIBJPEG) $(FLAGS) $(SRC)/sorters.cpp -c -o $(SRC)/sorters.o

mkbin:
	mkdir -p $(BIN)

clean: 
	rm -f *.o $(SRC)/*.o
	rm -rf $(BIN)
