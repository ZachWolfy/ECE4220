/* 	Name       : 	server_udp_broadcast.c
	Author     : 	Luis A. Rivera
	Description: 	Simple server (broadcast)
					ECE4220/7220		*/

#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <linux/if_link.h>
#include <ifaddrs.h>

#define MSG_SIZE 40			// message size
#define WHOIS "WHOIS"
#define VOTE "VOTE"
#define CHECK "#"


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
   int sock, length, n;
   int boolval = 1;			// for a socket option
   socklen_t fromlen;
   struct sockaddr_in server;
   struct sockaddr_in addr;
   char buffer[MSG_SIZE];	// to store received messages or messages to be sent.
	
	int lose = 0;
	int master = 0;
	char copy[MSG_SIZE], mygenerator[MSG_SIZE], random_s[10];
	int random, rcvascii, rcvrand, result;
	
	//declare randomization
	srand(time(0));
	
   if (argc < 2)
   {
	  printf("usage: %s port\n", argv[0]);
      exit(0);
   }

   sock = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
   if (sock < 0)
	   error("Opening socket");

   length = sizeof(server);			// length of structure
   bzero(&server,length);			// sets all values to zero. memset() could be used
   server.sin_family = AF_INET;		// symbol constant for Internet domain
   server.sin_addr.s_addr = INADDR_ANY;		// IP address of the machine on which
											// the server is running
   server.sin_port = htons(atoi(argv[1]));	// port number

   // binds the socket to the address of the host and the port number
   if (bind(sock, (struct sockaddr *)&server, length) < 0)
       error("binding");

   // change socket permissions to allow broadcast
   if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
   	{
   		printf("error setting socket options\n");
   		exit(-1);
   	}

   fromlen = sizeof(struct sockaddr_in);	// size of structure

	//getting IP for board
	struct ifaddrs *myNI;
	int family, s;
	char myIP[40];
	if (getifaddrs(&myNI) == -1) {
       perror("getifaddrs");
       exit(EXIT_FAILURE);
   }

   /* Walk through linked list, maintaining head pointer so we
      can free list later */

   for (struct ifaddrs *ifa = myNI; ifa != NULL; ifa = ifa->ifa_next) {
       if (ifa->ifa_addr == NULL)
           continue;

       family = ifa->ifa_addr->sa_family;
  /* For an AF_INET* interface address, display the address */

       if (family == AF_INET) {
			s = getnameinfo(ifa->ifa_addr,
                           (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                                 sizeof(struct sockaddr_in6),
                           myIP, NI_MAXHOST,
                           NULL, 0, NI_NUMERICHOST);
           if (s != 0) {
               printf("getnameinfo() failed: %s\n", gai_strerror(s));
               exit(EXIT_FAILURE);
           }
			
			if(strncmp(myIP, "192", 3) == 0)
			{
				printf("\t\taddress: <%s>\n", myIP);
			}
       }
   }

   freeifaddrs(myNI);

   while (1)
   {
	   // bzero: to "clean up" the buffer. The messages aren't always the same length...
	   bzero(buffer,MSG_SIZE);		// sets all values to zero. memset() could be used

	   // receive from a client
	   n = recvfrom(sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&addr, &fromlen);
       if (n < 0)
    	   error("recvfrom"); 

       printf("Received a datagram. It says: %s", buffer);

		//check who is master if I am master then send to client saying so.
		//if not don't send anyhthing
		if(strncmp(buffer, WHOIS, 5) == 0)
		{
			if(master == 1)
			{
				strncpy(mygenerator, "Wan on ", 7);
				strcat(mygenerator, myIP);
				strcat(mygenerator, " is the master\n");
				n = sendto(sock, mygenerator, 35, 0,
    		      (struct sockaddr *)&addr, fromlen);
				if (n  < 0)
					error("sendto WHOIS master");
				
			}
			else
			{
				n = sendto(sock, "\n", 35, 0,
    		      (struct sockaddr *)&addr, fromlen);
				if (n  < 0)
					error("sendto WHOIS not master");
			}
		}
		//voting who is the master. will send # ip random_number to everyone
		else if(strncmp(buffer, VOTE, 4) == 0)
		{
			//reset master and lose flag
			lose = 0;
			master = 1;
			
			//copy # ip
			strncpy(mygenerator, "# ", 2);
			strcat(mygenerator, myIP);
			strcat(mygenerator, " ");
			mygenerator[16] = '\0';
			
			//get random value generate
			random = 1 + rand() % 10;
			sprintf(random_s, "%d", random);
						
			//copy into string
			strcat(mygenerator, random_s);
			
			//send to socket
			n = sendto(sock, mygenerator, 20, 0,
    		      (struct sockaddr *)&addr, fromlen);
			if (n  < 0)
				error("sendto VOTE");
		}
		//compare if haven't lose to see who is the master
		else if(strncmp(buffer, CHECK, 1) == 0 && lose == 0)
		{
	
			//get random number from sender
			strcpy(copy, buffer+16);
			sscanf(copy, "%d", &rcvrand);
						
			//if receiver has higher generated number then I lose
			if(rcvrand > random)
			{
				lose = 1;
				master = 0;
			}
			//if I am higher than I become the master
			else if(rcvrand < random && lose == 0)
			{
				master = 1;
			}
			//if random number generated is the same then the person with
			//the highest IP wins
			else
			{printf("enter same\n");
				strncpy(copy, buffer+2, 13);
				result = strncmp(myIP, copy, 13);
				if(result > 0)
				{printf("enter win same\n");
					master = 1;
				}
				else
				{printf("enter lose same\n");
					master = 0;
					lose = 1;
				}
			}
			n = sendto(sock, "\n", 32, 0,
    		      (struct sockaddr *)&addr, fromlen);
			if (n  < 0)
				error("sendto");
		}
		else
		{
			n = sendto(sock, "Got a message. Was it from you?\n", 32, 0,
    		      (struct sockaddr *)&addr, fromlen);
			if (n  < 0)
				error("sendto");
		}
		memset(mygenerator, 0, sizeof(mygenerator));
		memset(random_s, 0, sizeof(random_s));
       // To send a broadcast message, we need to change IP address to broadcast address
       // If we don't change it (with the following line of code), the message
       // would be transmitted to the address from which the message was received.
	   // You may need to change the address below (check ifconfig)
       addr.sin_addr.s_addr = inet_addr("192.168.1.255");		// broadcast address

       /*n = sendto(sock, "Got a message. Was it from you?\n", 32, 0,
    		      (struct sockaddr *)&addr, fromlen);
       if (n  < 0)
    	   error("sendto");*/
   }

   return 0;
 }
