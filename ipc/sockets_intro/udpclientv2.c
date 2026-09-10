/* 
This is an example UDP client, that transmits a message to a chat server (udpserver.c)
*/

/**
 common.h includes commonly needed header files and helper functions
		like err_n_die(), which is a variant of printf.
		It also defines some values like MAXLINE and SERVER_PORT.
**/
#include "common.h" 
#include <netdb.h>



int main(int argc, char **argv)
{
	int					sockfd;
	int 				sendbytes;
	struct sockaddr_in	servaddr, fromaddr;
	char				sendline[MAXLINE + 1];
	char				recvline[MAXLINE + 1];
	socklen_t			servaddr_length, fromaddr_length;
	
	

	if (argc != 4) 
		err_n_die("usage: %s <server address> <port> <name>",argv[0]);
	else {
		for (int i=0; i < argc; i++)
		{
			printf("%d : %s\n", i, argv[i]);
		}
	}
	
	//setting up an address
	/*bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(SERVER_PORT);	
	
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
	{
		err_n_die("inet_pton error for %s ", argv[1]);
	}
	*/
	
	struct addrinfo hints, *result;
	int n;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; 		//IPv4 or IPv6 
	hints.ai_socktype = SOCK_DGRAM;		//We want to send datagrams
	hints.ai_protocol = 0; 				//any protocol
	
	if ( (n = getaddrinfo(argv[1], argv[2], &hints, &result)) != 0 )
		err_n_die("Couldn't resolve address!");
	
	
	
	//I want a "socket", but now use the information returned by getaddrinfo
	if ( (sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) < 0)
		err_n_die("Error while creating the socket!");
		
	//We've got our socket. Let's send something!
	sendbytes = strlen(argv[3]); 
	
	printf("Sending text \"%s\" to %s, port %s\n",argv[3],argv[1],argv[2]);
	//send the message
	if (sendto(sockfd, argv[3], sendbytes, 0, result->ai_addr, result->ai_addrlen) < 0)
		err_n_die("sendto error");
	
	freeaddrinfo(result);
	
	//Now read the server's response.
	if (recvfrom(sockfd, recvline, MAXLINE, 0, NULL,NULL) < 0)
		err_n_die("receive error");
	
	fprintf(stdout, "%s\n",recvline);
	

	exit(0); //end successfully!
}
