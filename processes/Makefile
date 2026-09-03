CC=gcc
CFLAGS=-Wall -g


BINS=forktest rawhelloarm hello exec1 exec2 exec3 exec4 orphans zombies file_descriptors pipes duplicating_fds #rawhellointel


all: $(BINS)

norva:
	echo 0 | sudo tee /proc/sys/kernel/randomize_va_space

rva:
	echo 2 | sudo tee /proc/sys/kernel/randomize_va_space


rawhellointel: rawsyscall-intel.S
	$(CC) $(CFLAGS) -nostdlib -no-pie -o rawhellointel rawsyscall-intel.S

rawhelloarm: rawsyscall-arm.S
	$(CC) $(CFLAGS) -nostdlib -no-pie -o rawhelloarm rawsyscall-arm.S

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(BINS) 
	$(RM) -r *.dSYM
