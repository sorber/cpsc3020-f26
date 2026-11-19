
CC=clang

ALLFLAGS= -g -Wall -std=c11
CFLAGS=-c
COMMON_OBJS = 
BINS = sniffer checksums httpex

all: $(BINS)

%: %.c 
	$(CC) $(ALLFLAGS) -o $@ $^

test:
	echo -n "HELLO World, Guys" | nc -u -N -q 0 localhost 9999

clean:
	rm -rf *.o $(BINS) *.dSYM
