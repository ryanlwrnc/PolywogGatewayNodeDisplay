#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include "polywog.h"

#define BUFFLEN 1024
#define PORT 4955
#define MAX_NODES 256
#define DATA_SIZE 26
#define MAX_AGE 30
#define SLEEP_TIME 1

int nodesAlive[MAX_NODES];

void initArray() {
   int i;

   for(i = 0; i < MAX_NODES; i++) {
      nodesAlive[i] = 0;
   }
}

void updateAge(BYTE *data) {
   int i;

   sleep(SLEEP_TIME);

   for (i = 0; i < DATA_SIZE; i++) {
      if (data[i] != 0)
         nodesAlive[data[i]] = MAX_AGE;
   }

   for (i = 0; i < MAX_NODES; i++) {
      if (nodesAlive[i] != 0)
         nodesAlive[i]--;
   }
}

void displayAliveNodes() {
   int i;
   
   printf("Alive nodes:\n");

   for (i = 0; i < MAX_NODES; i++) {
      if (nodesAlive[i] != 0)
         printf("%x\n", i);
   }
}

void main()
{

	struct sockaddr_in serverAddr, clientAddr;
	int sockfd, clientLen, i=0;
	char buffer[BUFFLEN];
   GW_PACKET *gwp;
   PW_PACKET pack_recv;
   PW_HDR hd_recv;   // srcAddr, dstAddr
   unsigned int net_num;
   BYTE srcAddr, dstAddr;
   BYTE data[DATA_SIZE];

   initArray(nodesAlive);

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

      gwp = (GW_PACKET *)buffer;
      net_num = gwp->gw_header.network_number;

      printf("Network # detected: %d\n", net_num);

      pack_recv = gwp->packet;
      hd_recv = pack_recv.header;

      memcpy(data, pack_recv.data, DATA_SIZE);

      updateAge(data);

      displayAliveNodes();

      srcAddr = hd_recv.srcAddr;
      dstAddr = hd_recv.dstAddr;

      printf("Source address: 0x%x\n", srcAddr);
      printf("Destination address: 0x%x\n", dstAddr);
      printf("\n");

		//printf("Client Data Received: %s\n", buffer);


		/*if((sendto(sockfd, buffer, BUFFLEN, 0, (struct sockaddr *)&clientAddr, clientLen)) == -1)
		{
			perror("sendto failed");
			exit(1);
		}*/

	}

	close(sockfd);

}
