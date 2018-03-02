CXX=g++
CXXFLAGS=-std=c++11 -O2 -fopenmp
BIN=bin/main

SRC=$(wildcard src/*.cpp)
OBJ=$(SRC:%.cpp=%.o)

all: $(OBJ)
	$(CXX) -o $(BIN) $^ -std=c++11 -O2 -fopenmp

%.o: %.c
	$(CXX) $@ -c $<

clean:
	rm -f src/*.o
	rm $(BIN)
