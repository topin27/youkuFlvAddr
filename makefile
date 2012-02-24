.phony: all clean

all: test

test: test.cpp youkuflvaddr.cpp
	g++ -g -Wall -o $@ $^

clean:
	rm -f test
