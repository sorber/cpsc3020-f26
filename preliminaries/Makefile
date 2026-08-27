
CC=clang
CFLAGS=-Wall -g

BINS=mystery1 mystery2 mystery3 mystery4 shim.so shimtest envtest envdemo


all: $(BINS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $?

shim.so: shim.c
	$(CC) $(CFLAGS) -shared -fPIC shim.c -o shim.so -ldl

clean:
	$(RM) $(BINS)
	$(RM) -r *.dSYM
