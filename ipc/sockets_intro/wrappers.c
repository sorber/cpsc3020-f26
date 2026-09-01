#include "common.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

/* these wrappers are slightly-modified versions of he wrappers
used in the UNP book. We use them just to keep code examples
more concise and readable */

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int n;

again:
	if ((n = accept(fd, sa, salenptr)) < 0)
	{
#ifdef EPROTO
		if (errno == EPROTO || errno == ECONNABORTED)
#else
		if (errno == ECONNABORTED)
#endif
			goto again;
		else
			err_n_die("accept error");
	}
	return (n);
}

void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (bind(fd, sa, salen) < 0)
		err_n_die("bind error");
}

void Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (connect(fd, sa, salen) < 0)
		err_n_die("connect error");
}

void Getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	if (getpeername(fd, sa, salenptr) < 0)
		err_n_die("getpeername error");
}

void Getsockname(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	if (getsockname(fd, sa, salenptr) < 0)
		err_n_die("getsockname error");
}

void Getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlenptr)
{
	if (getsockopt(fd, level, optname, optval, optlenptr) < 0)
		err_n_die("getsockopt error");
}

/* include Listen */
void Listen(int fd, int backlog)
{
	char *ptr;

	/*4can override 2nd argument with environment variable */
	if ((ptr = getenv("LISTENQ")) != NULL)
		backlog = atoi(ptr);

	if (listen(fd, backlog) < 0)
		err_n_die("listen error");
}
/* end Listen */

ssize_t
Recv(int fd, void *ptr, size_t nbytes, int flags)
{
	ssize_t n;

	if ((n = recv(fd, ptr, nbytes, flags)) < 0)
		err_n_die("recv error");
	return (n);
}

ssize_t
Recvfrom(int fd, void *ptr, size_t nbytes, int flags,
		 struct sockaddr *sa, socklen_t *salenptr)
{
	ssize_t n;

	if ((n = recvfrom(fd, ptr, nbytes, flags, sa, salenptr)) < 0)
		err_n_die("recvfrom error");
	return (n);
}

ssize_t
Recvmsg(int fd, struct msghdr *msg, int flags)
{
	ssize_t n;

	if ((n = recvmsg(fd, msg, flags)) < 0)
		err_n_die("recvmsg error");
	return (n);
}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
		   struct timeval *timeout)
{
	int n;

	if ((n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
		err_n_die("select error");
	return (n); /* can return 0 on timeout */
}

void Send(int fd, const void *ptr, size_t nbytes, int flags)
{
	if (send(fd, ptr, nbytes, flags) != (ssize_t)nbytes)
		err_n_die("send error");
}

void Sendto(int fd, const void *ptr, size_t nbytes, int flags,
			const struct sockaddr *sa, socklen_t salen)
{
	if (sendto(fd, ptr, nbytes, flags, sa, salen) != (ssize_t)nbytes)
		err_n_die("sendto error");
}

void Sendmsg(int fd, const struct msghdr *msg, int flags)
{
	unsigned int i;
	ssize_t nbytes;

	nbytes = 0; /* must first figure out what return value should be */
	for (i = 0; i < msg->msg_iovlen; i++)
		nbytes += msg->msg_iov[i].iov_len;

	if (sendmsg(fd, msg, flags) != nbytes)
		err_n_die("sendmsg error");
}

void Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	if (setsockopt(fd, level, optname, optval, optlen) < 0)
		err_n_die("setsockopt error");
}

void Shutdown(int fd, int how)
{
	if (shutdown(fd, how) < 0)
		err_n_die("shutdown error");
}

int Sockatmark(int fd)
{
	int n;

	if ((n = sockatmark(fd)) < 0)
		err_n_die("sockatmark error");
	return (n);
}

/* include Socket */
int Socket(int family, int type, int protocol)
{
	int n;

	if ((n = socket(family, type, protocol)) < 0)
		err_n_die("socket error");
	return (n);
}
/* end Socket */

void Socketpair(int family, int type, int protocol, int *fd)
{
	int n;

	if ((n = socketpair(family, type, protocol, fd)) < 0)
		err_n_die("socketpair error");
}

const char *
Inet_ntop(int family, const void *addrptr, char *strptr, size_t len)
{
	const char *ptr;

	if (strptr == NULL) /* check for old code */
		err_n_die("NULL 3rd argument to inet_ntop");
	if ((ptr = inet_ntop(family, addrptr, strptr, len)) == NULL)
		err_n_die("inet_ntop error"); /* sets errno */
	return (ptr);
}

void Inet_pton(int family, const char *strptr, void *addrptr)
{
	int n;

	if ((n = inet_pton(family, strptr, addrptr)) < 0)
		err_n_die("inet_pton error for %s", strptr); /* errno set */
	else if (n == 0)
		err_n_die("inet_pton error for %s", strptr); /* errno not set */

	/* nothing to return */
}

/* UNIX SYS CALL WRAPPERS */

void *
Calloc(size_t n, size_t size)
{
	void *ptr;

	if ((ptr = calloc(n, size)) == NULL)
		err_n_die("calloc error");
	return (ptr);
}

void Close(int fd)
{
	if (close(fd) == -1)
		err_n_die("close error");
}

void Dup2(int fd1, int fd2)
{
	if (dup2(fd1, fd2) == -1)
		err_n_die("dup2 error");
}

int Fcntl(int fd, int cmd, int arg)
{
	int n;

	if ((n = fcntl(fd, cmd, arg)) == -1)
		err_n_die("fcntl error");
	return (n);
}

void Gettimeofday(struct timeval *tv, void *foo)
{
	if (gettimeofday(tv, foo) == -1)
		err_n_die("gettimeofday error");
	return;
}

int Ioctl(int fd, int request, void *arg)
{
	int n;

	if ((n = ioctl(fd, request, arg)) == -1)
		err_n_die("ioctl error");
	return (n); /* streamio of I_LIST returns value */
}

pid_t Fork(void)
{
	pid_t pid;

	if ((pid = fork()) == -1)
		err_n_die("fork error");
	return (pid);
}

void *
Malloc(size_t size)
{
	void *ptr;

	if ((ptr = malloc(size)) == NULL)
		err_n_die("malloc error");
	return (ptr);
}

int Mkstemp(char *template)
{
	int i;
	if ((i = mkstemp(template)) < 0)
		err_n_die("mkstemp error");

	return i;
}

#include <sys/mman.h>

void *
Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
	void *ptr;

	if ((ptr = mmap(addr, len, prot, flags, fd, offset)) == ((void *)-1))
		err_n_die("mmap error");
	return (ptr);
}

int Open(const char *pathname, int oflag, mode_t mode)
{
	int fd;

	if ((fd = open(pathname, oflag, mode)) == -1)
		err_n_die("open error for %s", pathname);
	return (fd);
}

void Pipe(int *fds)
{
	if (pipe(fds) < 0)
		err_n_die("pipe error");
}

ssize_t
Read(int fd, void *ptr, size_t nbytes)
{
	ssize_t n;

	if ((n = read(fd, ptr, nbytes)) == -1)
		err_n_die("read error");
	return (n);
}

void Write(int fd, void *ptr, size_t nbytes)
{
	if (write(fd, ptr, nbytes) != nbytes)
		err_n_die("write error");
}
