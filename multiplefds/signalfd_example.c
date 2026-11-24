// for sigabbrev_np
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <signal.h>
#include <sys/signalfd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
// for sigabbrev_np
#include <string.h>

#define BUFSIZE 256


static int create_signal_fd();
static void my_raise(int sig);

int main(){
	int sfd = create_signal_fd();

	my_raise(SIGUSR1);
	printf("SIGUSR1 sent but not yet processed :p\n");
	my_raise(SIGINT);
	printf("SIGINT sent but we're still alive!( ˶ˆᗜˆ˵ )\n");
	my_raise(SIGTERM);
	printf("SIGTERM sent but we're still chilling!(⌐■_■)\n");

	printf("Gonna try to raise an invalid signal... Wish me luck!\n");
	my_raise(-1);

	struct signalfd_siginfo signal_buffers[2];
	const char *signal_name;
	int bytes_read;
	int signals_gotten;

	// we can process multiple signals at once, but how many we actually
	// get is known by checking how many bytes we actually read
	bytes_read = read(sfd, signal_buffers, sizeof(signal_buffers));
	signals_gotten = bytes_read/sizeof(struct signalfd_siginfo);

	// We should have 3 signals pending but our buffer only fits 2
	assert(signals_gotten == 2);
	signal_name = sigabbrev_np(signal_buffers[0].ssi_signo);
	// This can happen if the signal number is unknown
	assert(signal_name != NULL);
	printf("Wowie! We got signal %s, =O\n", signal_name);

	signal_name = sigabbrev_np(signal_buffers[1].ssi_signo);
	assert(signal_name != NULL);
	printf("Zowie! We got signal %s, :3 \n", signal_name);

	bytes_read = read(sfd, signal_buffers, sizeof(signal_buffers));
	signals_gotten = bytes_read/sizeof(struct signalfd_siginfo);

	// We should at least get the one signal we raised earlier
	// But it is also possible we got more signals
	assert(signals_gotten > 0);

	signal_name = sigabbrev_np(signal_buffers[0].ssi_signo);
	assert(signal_name != NULL);
	printf("%s, *Notices your signal* OwO whats dis.\n", signal_name);

	if (signals_gotten > 1) {
		// Someone sent us another signal outside of the ones we sent
		signal_name = sigabbrev_np(signal_buffers[1].ssi_signo);
		assert(signal_name != NULL);
		printf("what da heccc, I got more signals!!!!%s\n", signal_name);
	}

	// Side note: it is possible we receive the signals in a different order
	// than we sent
	return 0;
}


int create_signal_fd(){
	sigset_t mask;
	int sfd;

	// Block the signals we want to handle through the fd
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGTERM);
	sigaddset(&mask, SIGUSR1);

	if (sigprocmask(SIG_BLOCK, &mask, nullptr) == -1) {
		perror("sigprocmask");
		exit(EXIT_FAILURE);
	}

	// Create the signalfd
	sfd = signalfd(
		-1, // create a new signalfd
		&mask, // we wish to accept these signals through the fd
		SFD_CLOEXEC // close this fd on exec (security hardening)
	);
	if (sfd == -1) {
		perror("signalfd");
		exit(EXIT_FAILURE);
	}
	return sfd;
}


void my_raise(int sig){
	int ret = raise(sig);
	if (ret != 0){
		fprintf(stderr,"AUR NAUR!!!! I couldn't send signal %d. QwQ\n", sig);
	}
}