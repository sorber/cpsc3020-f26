
CC=clang

ALLFLAGS= -g
CFLAGS=-c
COMMON_OBJS = common.o wrappers.o
BINS = tcpclient tcpclientv2 tcpserver udpclient udpclientv2 udpserver

all: $(COMMON_OBJS) $(BINS)
	
#Common wrappers and definitions that all of the other examples use.
%.o: %.c common.h
	$(CC) $(CFLAGS) $(ALLFLAGS) $<

%: %.c $(COMMON_OBJS)
	$(CC) $(ALLFLAGS) -o $@ $^


clean:
	rm -rf *.o $(BINS)
