EXE = peer
CFLAGS = -Wall
CXXFLAGS = -Wall
LDLIBS = -lm
CC = gcc
CXX = g++

.PHONY: all
all: $(EXE)

# Implicit rules defined by Make, but you can redefine if needed
#
peer: main.c
	$(CC) $(CFLAGS) main.c $(LDLIBS) -o peer 

.PHONY: clean
clean:
	rm -f $(EXE)

