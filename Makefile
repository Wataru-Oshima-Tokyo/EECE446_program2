EXE = p2p _p2p_sample
CFLAGS = -Wall
CXXFLAGS = -Wall
LDLIBS =
CC = gcc
CXX = g++

.PHONY: all
all: $(EXE)

# Implicit rules defined by Make, but you can redefine if needed
#
p2p: main.c
	$(CC) $(CFLAGS) main.c $(LDLIBS) -o p2p
_p2p_sample: sub.c
	$(CC) $(CFLAGS) sub.c $(LDLIBS) -o _p2p_sample
#
# OR
#
#h1-counter: h1-counter.cc
#       $(CXX) $(CXXFLAGS) h1-counter.cc $(LDLIBS) -o h1-counter

.PHONY: clean
clean:
	rm -f $(EXE)
	rm -f main
