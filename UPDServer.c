#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define BUFFLEN 1024
#define PORT 4955

void main()
{

	struct sockaddr_in serverAddr, clientAddr;
	int sockfd, clientLen, i=0;
	char buffer[BUFFLEN];

	/* Create server socket */
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket failed:");
		exit(1);
	}

	/* clear serverAddr structue */
	memset((char *) &serverAddr, 0, sizeof(serverAddr));

	serverAddr.sin_family		= AF_INET;		        // For IPV4 internet protocol 
	serverAddr.sin_port		    = htons(PORT);  		// Assign port on which server will listen
	serverAddr.sin_addr.s_addr	= htonl(INADDR_ANY);	// Any adress available on system


	/*Bind name & address to the socket */
	if((bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr))) == -1)
	{
		perror("Bind failed:");
		exit(1);
	}

	clientLen = sizeof(clientAddr);

	while(1)
	{

		printf("Waiting for data....\n");

		/*Receive data sent from the client*/		
		if((recvfrom(sockfd, buffer, BUFFLEN, 0, (struct sockaddr *) &clientAddr, &clientLen)) == -1)
		{
			perror("recvfrom failed");
			exit(1);
		}

		printf("Client Data Received: %s\n", buffer);


		if((sendto(sockfd, buffer, BUFFLEN, 0, (struct sockaddr *)&clientAddr, clientLen)) == -1)
		{
			perror("sendto failed");
			exit(1);
		}

	}

	close(sockfd);

}
