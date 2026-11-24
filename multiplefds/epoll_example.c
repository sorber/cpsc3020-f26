#include "examples.h"
#include <stdint.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>

#define WRITERS 15
#define BUFSIZE 256
// Lets handle a max number of 10 events at a time
#define EVENT_COUNT 10

static int create_epoll();
static int create_eventfd(int flags);
static int create_writer(int event_fd);
static void set_nonblock(int fd);
static void reap_all_children();

int main() {
	int epoll_fd = create_epoll();
	// Create an event with semaphore semantics
	// It's a suprise tool that will help us later
	int event_fd = create_eventfd(EFD_SEMAPHORE);

	int read_fds[WRITERS];
	for (int i = 0; i < WRITERS; i++){
		int read_fd = create_writer(event_fd);
		read_fds[i] = read_fd;
		struct epoll_event ev = {
			.events = EPOLLIN, // We will use this fd for reading
			.data = {// Data that will be returned in epoll_wait
				.fd = read_fd // We want to know what fd is ready
			}
		};
		check_ret(epoll_ctl(epoll_fd, EPOLL_CTL_ADD,read_fd,&ev));
	}

	size_t counter = 0;
	struct epoll_event events[EVENT_COUNT];
	while (true) {
		if (counter == 6){
			printf("Telling 5 children to stop and breaking the loop\n");
			// After 5 iterations, lets tell some more workers to stop and exit this loop
			uint64_t semaphore_increment = 5;
			check_ret(write(event_fd, &semaphore_increment, sizeof(uint64_t)));
			delay_ms(1000);
			break;
		}
		if (counter == 1) {
			printf("Telling 5 children to stop and waiting a lil bit\n");
			uint64_t semaphore_increment = 5;
			check_ret(write(event_fd, &semaphore_increment, sizeof(uint64_t)));
			delay_ms(1000);
		}
		int num_of_events = check_ret(epoll_wait(epoll_fd, events, EVENT_COUNT, -1));

		for (int i = 0; i < num_of_events; i++){
			if (events[i].events & (EPOLLERR | EPOLLRDHUP | EPOLLHUP)) {
				// An error happened to our fd, could mean that the other end of a pipe
				// or socket was closed
				printf("FD: %d got an error, unsubscribing from this lame-o\n", events[i].data.fd);
				// unsubscribing from this lame-o as promised
				check_ret(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd,nullptr));
			} else if (events[i].events & EPOLLIN) {
				// Add room for an extra null character case the message gets cuts off (too big for our buf)
				char buf[BUFSIZE+1];
				int read_count = check_ret(read(events[i].data.fd,buf, BUFSIZE));
				buf[read_count] = '\0'; // Null terminate if message wasn't already
				printf("FD: %d is sending us a message... they say: \"%s\"\n",
					events[i].data.fd,
					buf
				);
			}
		}
		counter++;
	}

	printf("Closing all fd, our hard-working children will notice and should error gracefully\n");
	check_ret(close(epoll_fd));
	check_ret(close(event_fd));
	for(int i = 0; i < WRITERS; i++){
		check_ret(close(read_fds[i]));
	}
	reap_all_children();
	printf("Done! =D\n");

}

int create_epoll(){
	// Use create1 instead of obsolete create.
	int epoll_fd = check_ret(epoll_create1(0));
	/* can also do it without my handy dandy macro i supposse....
	int epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	if (epoll_fd == -1) {
		perror("epoll_create1 failed");
		exit(EXIT_FAILURE);
	}
	*/
	return epoll_fd;
}

int create_eventfd(int flags){
	return check_ret(
		eventfd(
			0, // Initial value
			flags
		)
	);
}

typedef union pipe_t{
	int fds[2];
	struct{
		int read_fd;
		int write_fd;
	};
}pipe_t;

int create_writer(int event_fd){
	static int child_num = 0;
	pipe_t pipes;
	check_ret(pipe(pipes.fds));
	if (fork() != 0){
		// Parent
		child_num++;
		close(pipes.write_fd);
		return pipes.read_fd;
	}
	close(pipes.read_fd);

	// Multiple readers so it is possible one will respond to the semaphore
	// Before us
	set_nonblock(event_fd);

	int epoll_fd = create_epoll();
	struct epoll_event ev = {
		.events = EPOLLOUT, // We will use this fd for writing
		.data = {// Data that will be returned in epoll_wait
			.fd = pipes.write_fd // We want to know what fd is ready
		}
	};

	check_ret(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipes.write_fd, &ev));
	ev.events = EPOLLIN;
	ev.data.fd = event_fd;
	check_ret(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_fd, &ev));

	while (true){
		// We can respond to multiple events at once but let's just do one at a time
		// Also block indefinetly
		int num_of_fds = check_ret(epoll_wait(epoll_fd,&ev,1, -1));
		assert(num_of_fds == 1); // Doesn't make sense for this to not be 1
		if (ev.events & EPOLLERR){
			fprintf(stderr,"<child %d> Error happened with our fd %d\n", child_num,ev.data.fd);
			exit(0);
		}
		char buffer[BUFSIZE];
		if (ev.events & EPOLLOUT) { // A writing fd is available
			int bytes_to_write =  check_ret(snprintf(
				buffer,
				BUFSIZE,
				"Message from child %d",
				child_num
			));
			check_ret(write(ev.data.fd,buffer, BUFSIZE));
		} else if (ev.events & EPOLLIN) { // a reading fd is available
			// The only reader fd available is the event fd
			uint64_t event;
			int read_ret = read(ev.data.fd, &event, sizeof(uint64_t));
			if (read_ret == -1 && errno == EAGAIN) {
				// Someone got the semaphore before us.
				continue;
			}
			fprintf(stderr,"Child %d exiting...\n", child_num);
			// We got the semaphore... that means we should stop
			close(epoll_fd);
			close(event_fd);
			close(pipes.write_fd);
			exit(0);
		}
	}
}

void set_nonblock(int fd){
	int flags = check_ret(fcntl(fd, F_GETFL));
	check_ret(fcntl(fd, F_SETFL, flags | O_NONBLOCK));
}

void reap_all_children() {
	int status;
	pid_t pid;

	// Loop until waitpid returns -1 (no children left or error)
	while ((pid = waitpid(-1, &status, 0)) > 0) {
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
			printf("Child %d exited with status %d\n", pid, WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			printf("Child %d killed by signal %d\n", pid, WTERMSIG(status));
		}
	}

	// ECHILD means all children have been reaped; anything else is an actual error
	if (pid == -1 && errno != ECHILD) {
		perror("waitpid failed");
	}
}

