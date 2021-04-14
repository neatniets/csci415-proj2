.POSIX:

.SUFFIXES:
.SUFFIXES: .cpp .o

CC=g++
CFLAGS=-g -std=c++11
LDFLAGS=-lcrypt

SRC=rainbow.cpp hash-reduce-test.cpp rbtable.cpp
OBJ=$(SRC:.cpp=.o)

all:

clean:
	rm -f $(OBJ) hash-reduce-test.out

.cpp.o:
	$(CC) $(CFLAGS) -c -o $@ $<

hash-reduce-test.out: rainbow.o hash-reduce-test.o
	$(CC) $(LDFLAGS) -o $@ rainbow.o hash-reduce-test.o

rainbow.cpp: rainbow.hpp

rbtable.cpp: rbtable.hpp rainbow.hpp

.PHONY: all clean
