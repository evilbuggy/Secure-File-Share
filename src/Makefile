CC = g++
INC_DIR = -Iinclude
CFLAGS = -Wall $(INC_DIR)

all: secureFileShare

secureFileShare: main.o ./src/lint.o ./src/aes.o
	$(CC) $(CFLAGS) main.o ./src/lint.o ./src/aes.o -o secureFileShare

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp -o main.o

./src/aes.o: ./src/aes.cpp
	$(CC) $(CFLAGS) -c ./src/aes.cpp -o ./src/aes.o

./src/lint.o: ./src/lint.cpp
	$(CC) $(CFLAGS) -c ./src/lint.cpp -o ./src/lint.o

clean:
	rm -rf *.o ./src/*.o secureFileShare
