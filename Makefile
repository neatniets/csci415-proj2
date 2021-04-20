.POSIX:

.SUFFIXES:
.SUFFIXES: .cpp .o

CC=g++
CFLAGS=-g -std=c++11 -Wall
LDFLAGS=-lcrypt

SRC=rainbow.cpp hash-reduce-test.cpp rbtable.cpp build-table.cpp Crack.cpp
OBJ=$(SRC:.cpp=.o)
EXE=crack hash-reduce-test.out build-table

all: build-table crack

clean:
	rm -f $(OBJ) $(EXE)

.cpp.o:
	$(CC) $(CFLAGS) -c -o $@ $<

crack: Crack.o rainbow.o
	$(CC) $(LDFLAGS) -o $@ Crack.o rainbow.o

hash-reduce-test.out: rainbow.o hash-reduce-test.o
	$(CC) $(LDFLAGS) -o $@ rainbow.o hash-reduce-test.o

build-table: build-table.o rainbow.o rbtable.o
	$(CC) $(LDFLAGS) -o $@ build-table.o rainbow.o rbtable.o

rainbow.cpp: rainbow.hpp

rbtable.cpp: rbtable.hpp rainbow.hpp

build-table.cpp: rbtable.hpp

Crack.cpp: rainbow.hpp

.PHONY: all clean
