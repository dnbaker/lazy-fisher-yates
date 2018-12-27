CXX?=g++

all: $(patsubst %.cpp,%,$(wildcard *.cpp))
CXXLAGS=-O3 -std=c++14 -march=native -mavx2

%: %.cpp
	$(CXX) $(CXXLAGS) $< -o $@ -I.
