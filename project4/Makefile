
CC=clang

ALLFLAGS= -g -Wall -std=c11
CFLAGS=-c
BINS = test

all: $(BINS)
	

%: %.c 
	$(CC) $(ALLFLAGS) -o $@ $^


clean:
	rm -rf *.o $(BINS) *.dSYM
