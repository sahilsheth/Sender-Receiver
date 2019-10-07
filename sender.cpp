#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "msg.h"    /* For the message struct */
using namespace std;

/* The size of the shared memory segment */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void* sharedMemPtr;

 //Message for the sender
fileNameMsg msg;

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory
 * @param msqid - the id of the allocated message queue
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr)
{
	/* DONE:
        1. DONE: Create a file called keyfile.txt containing string "Hello world" (you may do
 	    so manually or from the code).
	2. Use ftok("keyfile.txt", 'a') in order to generate the key.
	3. Use will use this key in the TODO's below. Use the same key for the queue
	   and the shared memory segment. This also serves to illustrate the difference
 	   between the key and the id used in message queues and shared memory. The key is
	   like the file name and the id is like the file object.  Every System V object
	   on the system has a unique id, but different objects may have the same key.
	*/
    ofstream keyfile("keyfile.txt");
    keyfile << "Hello world" <<endl;
    //Taken from slide 63:
    key_t key = ftok("keyfile.txt",'a');

    //error check
    if(key < 0)
       {
           perror("ftok");
           cout << "Error getting key";
           exit(1);
       }

	/* DONE: Get the id of the shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE */
  shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, 0666 | IPC_CREAT);
	/* DONE: Attach to the shared memory */
  sharedMemPtr = shmat(shmid, NULL, 0);
	/* DONE: Attach to the message queue */
  //Slide 64
  msqid = msgget(key, 0666 | IPC_CREAT);
  //Slide 67, places message into the queue
  msgsnd(msqid, &msg, sizeof(msg) - sizeof(long), 0);
	/* DONE: Store the IDs and the pointer to the shared memory region in the corresponding function parameters */
  //Source: https://www.tldp.org/LDP/lpg/node36.html
  //Performs control operations on a message queue
  //msgctl(shmid, msqid, 0);
  init(shmid, msqid, sharedMemPtr);
}

/**
 * Performs the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */
void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	/* DONE: Detach from shared memory */
    //Taken from slide 62
    shmdt(sharedMemPtr);
}

/**
 * The main send function
 * @param fileName - the name of the file
 * @return - the number of bytes sent
 */
unsigned long sendFile(const char* fileName)
{

	/* A buffer to store message we will send to the receiver. */
	message sndMsg;

	/* A buffer to store message received from the receiver. */
	ackMessage rcvMsg;

	/* The number of bytes sent */
	unsigned long numBytesSent = 0;

	/* Open the file */
	FILE* fp =  fopen(fileName, "r");

	/* Was the file open? */
	if(!fp)
	{
		perror("fopen");
		exit(-1);
	}

	/* Read the whole file */
	while(!feof(fp))
	{
		/* Read at most SHARED_MEMORY_CHUNK_SIZE from the file and
 		 * store them in shared memory.  fread() will return how many bytes it has
		 * actually read. This is important; the last chunk read may be less than
		 * SHARED_MEMORY_CHUNK_SIZE.
 		 */
		if((sndMsg.size = fread(sharedMemPtr, sizeof(char), SHARED_MEMORY_CHUNK_SIZE, fp)) < 0)
		{
			perror("fread");
			exit(-1);
		}

		/* TODO: count the number of bytes sent. */

		/* TODO: Send a message to the receiver telling him that the data is ready
 		 * to be read (message of type SENDER_DATA_TYPE).
 		 */

		/* TODO: Wait until the receiver sends us a message of type RECV_DONE_TYPE telling us
 		 * that he finished saving a chunk of memory.
 		 */
	}


	/** TODO: once we are out of the above loop, we have finished sending the file.
 	  * Lets tell the receiver that we have nothing more to send. We will do this by
 	  * sending a message of type SENDER_DATA_TYPE with size field set to 0.
	  */
    
    //Similar to what was in the recv.cpp, unsure if correct
    struct message sendMessage;
    sendMessage.mtype = SENDER_DATA_TYPE;
    sendMessage.size = 0;
    msgsnd(msqid,&sendMessage.mtype,sendMessage.size,0);
	/* Close the file */
	fclose(fp);

	return numBytesSent;
}

/**
 * Used to send the name of the file to the receiver
 * @param fileName - the name of the file to send
 */
void sendFileName(const char* fileName)
{
	/* Get the length of the file name */
	int fileNameSizes = strlen(fileName);

	/* TODO: Make sure the file name does not exceed
	 * the maximum buffer size in the fileNameMsg
	 * struct. If exceeds, then terminate with an error.
	 */
	//MAX_FILE_NAME_SIZE, according to msg.h, is the max size of the file
	if(fileNameSizes > MAX_FILE_NAME_SIZE)
	{
		perror("size of file is too long");
		exit(-1);
	}
	/* TODO: Create an instance of the struct representing the message
	 * containing the name of the file.
	 */
         fileNameMsg msgs;

	/* TODO: Set the message type FILE_NAME_TRANSFER_TYPE */
	//name "m" stands for unsigned long 
	
	msgs.mtype = FILE_NAME_TRANSFER_TYPE;

	/* TODO: Set the file name in the message */
	//part of code from https://stackoverflow.com/questions/57081303/i-have-encountered-buffer-overflow-array-index-out-of-bounds-error
	strncpy(msgs.fileName, fileName, fileName + 1);

	/* TODO: Send the message using msgsnd */
	if (msgsnd(msqid, &msgs, sizeof(fileNameMsg) - sizeof(long), 0) < 0)
	{
		perror("msgsnd");
		exit(-1);
	}
    }
}


int main(int argc, char** argv)
{

	/* Check the command line arguments */
	if(argc < 2)
	{
		fprintf(stderr, "USAGE: %s <FILE NAME>\n", argv[0]);
		exit(-1);
	}

	/* Connect to shared memory and the message queue */
	init(shmid, msqid, sharedMemPtr);

	/* Send the name of the file */
        sendFileName(argv[1]);

	/* Send the file */
	fprintf(stderr, "The number of bytes sent is %lu\n", sendFile(argv[1]));

	/* Cleanup */
	cleanUp(shmid, msqid, sharedMemPtr);

	return 0;
}
