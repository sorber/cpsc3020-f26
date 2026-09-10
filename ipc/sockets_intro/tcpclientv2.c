/* 
This is an example TCP client, that transmits a message to a chat server (tcpserver.c)
*/

/**
 common.h includes commonly needed header files and helper functions
		like err_n_die(), which is a variant of printf.
		It also defines some values like MAXLINE that I use a lot.
**/
#include "common.h" 



int main(int argc, char **argv)
{
	int					sockfd, n;
	int 				sendbytes;
	struct sockaddr_in	servaddr;
	char				sendline[MAXLINE + 1];
	char				recvline[MAXLINE + 1];
	
	

	if (argc != 3) 
		err_n_die("usage: %s <server address> <name>",argv[0]);
	

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	int reuse = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	
	//set the receive timeout on the socket.
	struct timeval rtimeout;
	rtimeout.tv_sec = 5;
	rtimeout.tv_usec = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &rtimeout, sizeof(rtimeout));
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(SERVER_PORT);	/* chat server */
	
	
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
		
	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	//We're connected. Prepare the message.
	sprintf(sendline, "%s says Hi!\n", argv[2]);
	sendbytes = strlen(sendline); 
	
	//Send the message
	Write(sockfd, sendline, sendbytes);
	
	//Now read the server's response.
	while ( (n = Read(sockfd, recvline, MAXLINE)) > 0) 
	{
		recvline[n] = 0;	/* null terminate */
		if (fputs(recvline, stdout) == EOF)
			err_n_die("fputs error");
		
	}
	
	//no longer needed, since the wrapper checks this condition
	//if (n < 0)
	//	err_n_die("read error");
	

	exit(0); //end successfully!
}
