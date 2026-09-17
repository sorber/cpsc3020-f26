
CC=clang

ALLFLAGS= -g -Wall -std=c11
CFLAGS=-c
COMMON_OBJS = common.o wrappers.o
BINS = tcpclient tcpclientv2 tcpclient_ipv6 tcpserver udpclient udpclientv2 udpclient_ipv6 udpserver activityserver getaddrinfo_ex unix_domain_sockets

all: $(COMMON_OBJS) $(BINS)
	
#Common wrappers and definitions that all of the other examples use.
%.o: %.c common.h
	$(CC) $(CFLAGS) $(ALLFLAGS) $<

%: %.c $(COMMON_OBJS)
	$(CC) $(ALLFLAGS) -o $@ $^


clean:
	rm -rf *.o $(BINS) *.dSYM
