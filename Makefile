.POSIX:

.SUFFIXES:
.SUFFIXES: .cpp .o

CC=g++
CFLAGS=-g -std=c++11

SRC=rainbow.cpp
OBJS=$(SRC:.cpp=.o)

all:

clean:
	rm -f $(OBJ)

.cpp.o:
	$(CC) $(CFLAGS) -c -o $@ $<

rainbow.cpp: rainbow.hpp

.PHONY: all clean
