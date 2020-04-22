#include <sys/types.h>
#include <sys/socket.h>
#include "list.h"
#include <rtthreads.h>
#include <RttCommon.h>
#include <netdb.h>

/* Declare outgoing list */
LIST *outgoing_list;

/* Declare incoming list */
LIST *incoming_list;

/* Decare semaphore for outgoing list */
RttSem outgoing_sem;

/* Decare semaphore for incoming list */
RttSem incoming_sem;

RttSem outgoing_mutex;
RttSem incoming_mutex;

/* Socket info */
int send_sockfd;
int receive_sockfd;
struct addrinfo *p2;

/* This thread dequeues an itme off of the outgoing list and
 * sends it to the other user. It will also call print in order 
 * to print to the orginil users screen */
void sendMsg(void* theirinfo) {
	/*
	 * Get outgoing_sem lock
	 * Get item from outgoing_list
	 * Make call to sendto();
	 * check return values
	 * free outgoing_sem lock 
	*/
	char* sendmsg;	
	int rv;	
	
	/*printf("\nSend message thread created\n"); */
	rv = RttSleep(2);	
	if ( rv != 0 ) {
		printf("error sleeping thread");
	}	
	while(1) {
		RttP(outgoing_sem);
		RttP(outgoing_mutex);
		if (ListCount(outgoing_list) > 0 ) {
			sendmsg = ListTrim(outgoing_list);
			sendto(send_sockfd, sendmsg, strlen(sendmsg) +1, 0, p2->ai_addr, p2->ai_addrlen);
		}
		RttV(outgoing_mutex);
	}
}

/* This function receives a message from the other user and adds
 * it to the incoming list. It will then allow print thread to access
 * the incoming list and print the message to the screen */
void receiveMsg(void* arg) {
	  /*
         * Get incoming_sem lock
         * Get item from incoming_list
         * Make call to receive();
         * check return values
         * free incoming_sem lock
        */
	char buff[100];
	int rv;	
	
	/* printf("\nmessage receive thread started\n"); */
	while(1) {
		RttP(incoming_mutex);
		rv = recv(receive_sockfd, buff, 100, 0);
		if (rv > 0) {
			ListAdd(incoming_list, buff);
			RttV(incoming_sem);		
		}
		rv = RttSleep(1);
		if ( rv != 0 ) {
                	printf("error sleeping thread");
        	}	
		RttV(incoming_mutex);
	}

}


/* This thread takes input from the keyboard and adds the message
 * to the outgong queue. It then gives up the outgoing list lock so 
 * the send function can get the message from the list */
void readMsg(void* arg) {
	 /*
         * Get outgoing_sem lock
         * Get input from keyboard
	 * Add keyboard input to outgoing list
         * free outgoing_sem lock
	 * print input that was just typed in
        */
	int rv;
	char* buff[100];
	
	/*printf("\nMessage input thread started\n"); */
	while(1) {
		RttP(outgoing_mutex);
		rv = read(0, (void*) buff, 100);
		if( rv > -1 ) {
			ListAdd(outgoing_list, (void*) buff);
			RttV(outgoing_sem);
		}
		rv = RttSleep(1);
		if (rv != 0 ) {
			printf("error sleeping thread");
                }
		RttV(outgoing_mutex);
	}

}

/* This thread will dequeue the message off of the incoming list
 * and print it to the screen. It will then give up the incoming 
 * lock so that the receive thread can access the incoming list.
 * It will also used to print send items from the outgoing list */
void writeMsg(void *arg) {
  /*
         * Get incoming_sem lock
         * Get item from incoming_sem
         * free outgoing_sem lock
	 * print item to screen
        */
	char* msg;
	int rv;
	
	/* printf("\nMessage print thread started\n"); */
	rv = RttSleep(2);
	if (rv != 0 ) {
                printf("error sleeping thread");
                }
	while(1){
		RttP(incoming_sem);
		RttP(incoming_mutex);	
		ListFirst(incoming_list);
	
		msg = ListTrim(incoming_list);
			
		printf("%s", msg);
	
		RttV(incoming_mutex);
	}
	
}


/* 
 * 
 * Example program call
 * s-chat 30001 peon34 39001 */
int  mainp(int argc,void* argv[] ) {
	
	struct addrinfo hints, hints2, *p, *servinfo, *otherinfo;	
	int retVal;
	char *myPort;
	char *theirPort;
	char *theirName;
	
	/* define threads */
	RttThreadId send;
	RttThreadId receive;
	RttThreadId read;
	RttThreadId write;
	
	/* define struct for sched info */
	RttSchAttr threadSch;
	
	/* check input args */
	if(argc < 3) {
		printf("No enough arguments provided");
	}	
 	/* create lists and semaphores */      
	incoming_list = ListCreate();
        outgoing_list = ListCreate();

	RttAllocSem(&incoming_mutex, 1, RTTPRIORITY);
        RttAllocSem(&outgoing_mutex, 1, RTTPRIORITY);
	RttAllocSem(&outgoing_sem, 0, RTTPRIORITY);	
	RttAllocSem(&incoming_sem, 0, RTTPRIORITY);
	
	/* define the thread scheduling info */
	threadSch.startingtime = RTTZEROTIME;
	threadSch.priority = RTTNORM;
       	threadSch.deadline = RTTNODEADLINE;	
	
	/* map the input args */
	myPort = argv[1];
 	theirName = argv[2];
	theirPort = argv[3];
	
	/* prepare for adderinfo calls */
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
		
	memset(&hints2, 0, sizeof hints2);
	hints2.ai_family = AF_UNSPEC;
        hints2.ai_socktype = SOCK_DGRAM;


	/* Receive  Adder info */ 	
	/*
	 * Make call to addrinfo() 
	 */ 
	retVal = getaddrinfo(NULL, myPort, &hints, &servinfo);
	if(retVal !=  0 ) {
		return 1;
	}

	/* Make call to socket */
	for( p = servinfo; p != NULL; p = p->ai_next) {
		if ( (receive_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1 ) {
			perror("listener: socket ");
			continue;
		}
		   /* Make call to bind */
		if ( (bind(receive_sockfd, p->ai_addr, p->ai_addrlen)) == -1 ) {
			close(receive_sockfd);
			perror("listener: bind");
			continue;
		}
		break;

        }
	
	if(p == NULL) {
		printf("failed to bind to listening socket");
		return 1;
	}
		
	
	/* Send AdderInfo */ 
	/* Make get adderinfo */
	retVal = getaddrinfo(theirName, theirPort, &hints2, &otherinfo);
        if(retVal <  0 ) {
                return 1;
        }

	for( p2 = otherinfo; p2 != NULL; p2 = p2->ai_next) {
                if ( (send_sockfd = socket(p2->ai_family, p2->ai_socktype, p2->ai_protocol)) == -1 ) {
                        perror("sender: socket ");
                        continue;
                }
                  
                break;

        }
      
        if(p2 == NULL) {
                printf("failed to bind to send  socket");
                return 1;
        }
	 /* use fnctl to set NONBLOCK */	
	retVal = fcntl(send_sockfd, F_SETFL, O_NONBLOCK);
        if(retVal < 0 ) {
                perror("input fcntl");
        }
	
	retVal = fcntl(0, F_SETFL, O_NONBLOCK);
        if(retVal < 0 ) {
                perror("input fcntl");
        }	
	
	retVal = fcntl(1, F_SETFL, O_NONBLOCK);
        if(retVal < 0 ) {
                perror("input fcntl");
        }
	
	retVal = fcntl(receive_sockfd, F_SETFL, O_NONBLOCK);
        if(retVal < 0 ) {
                perror("input fcntl");
        }
	
	/* create threads */
        RttCreate(&send, (void(*)())sendMsg, 15000, "send", NULL, threadSch, RTTUSR);
	RttCreate(&receive, (void(*)())receiveMsg, 15000, "rec",NULL, threadSch, RTTUSR);
	RttCreate(&read, (void(*)())readMsg, 15000, "rec", NULL, threadSch, RTTUSR);
	RttCreate(&write, (void(*)())writeMsg, 15000, "writ", NULL, threadSch, RTTUSR);
	printf("Starting Chat....\n");
	return 0;	
}
