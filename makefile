CXX=g++
CXXFLAGS=-std=c++11 -O2 -fopenmp
BIN=main

SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)

all: $(OBJ)
	$(CXX) -o $(BIN) $^ -std=c++11 -O2 -fopenmp

%.o: %.c
	$(CXX) $@ -c $<

clean:
	rm -f *.o
	rm $(BIN)
