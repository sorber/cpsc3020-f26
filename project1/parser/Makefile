CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -g

all: parser_demo

parser_demo: parser.o demo.o
	$(CC) $(CFLAGS) -o $@ parser.o demo.o

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

demo.o: demo.c parser.h
	$(CC) $(CFLAGS) -c demo.c

test: parser_demo
	./tests.sh

clean:
	rm -f parser_demo *.o

.PHONY: all test clean
