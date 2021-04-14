.POSIX:

.SUFFIXES:
.SUFFIXES: .cpp .o

CC=g++
CFLAGS=-g -std=c++11
LDFLAGS=-lcrypt

SRC=rainbow.cpp hash-reduce-test.cpp rbtable.cpp build-table.cpp
OBJ=$(SRC:.cpp=.o)

all: build-table

clean:
	rm -f $(OBJ) hash-reduce-test.out build-table

.cpp.o:
	$(CC) $(CFLAGS) -c -o $@ $<

hash-reduce-test.out: rainbow.o hash-reduce-test.o
	$(CC) $(LDFLAGS) -o $@ rainbow.o hash-reduce-test.o

build-table: build-table.o rainbow.o rbtable.o
	$(CC) $(LDFLAGS) -o $@ build-table.o rainbow.o rbtable.o

rainbow.cpp: rainbow.hpp

rbtable.cpp: rbtable.hpp rainbow.hpp

build-table.cpp: rbtable.hpp

.PHONY: all clean
