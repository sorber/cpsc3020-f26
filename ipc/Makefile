
CC=clang
CFLAGS=-Wall -g


BINS=signals shmem shmem2 named_pipe_sender named_pipe_recver

all: $(BINS)


%: %.c
	$(CC) $(CFLAGS) -o $@ $?

	
clean:
	$(RM) $(BINS)
	$(RM) -r *.dSYM


