#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include "msg.h"    /* For the message struct */

using namespace std;

/* The size of the shared memory segment */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void *sharedMemPtr = NULL;


/**
 * The function for receiving the name of the file
 * @return - the name of the file received from the sender
 */
string recvFileName()
{
	/* The file name received from the sender */
	string fileName;
        
	/* TODO: declare an instance of the fileNameMsg struct to be
	 * used for holding the message received from the sender.
     
         */
        /* TODO: Receive the file name using msgrcv() */
	
	/* TODO: return the received file name */
    //not sure if it works yet
    fileNameMsg mess;
    

    if(msgrcv(msqid, &mess, sizeof(fileNameMsg) - sizeof(long), FILE_NAME_TRANSFER_TYPE, 0) < 0)
    {
        perror("msgsnd");
        exit(-1);
    }
    
    fileName = mess.fileName;
    //------------------------------------//

        return fileName;
}
 /**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 * @param msqid - the id of the shared memory
 * @param sharedMemPtr - the pointer to the shared memory
 */
void init(int& shmid, int& msqid, void*& sharedMemPtr)
{
	
	/* TODO: 
        1. Create a file called keyfile.txt containing string "Hello world" (you may do
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
    
    if(key < 0)
       {
           perror("ftok");
           cout << "Error getting key";
           exit(1);
       }

	/* TODO: Allocate a shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE. */
	// Taken from http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/shm/shmget.html
       //not completed
       
       shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, 0666 | IPC_CREAT);

       
       
       
	/* TODO: Attach to the shared memory */
       
       sharedMemPtr = shmat(shmid, NULL, 0);

	
	/* TODO: Create a message queue */
       
       //from https://www.geeksforgeeks.org/ipc-using-message-queues/
       
       msqid = msgget(key, 0666 | IPC_CREAT);
	
	/* TODO: Store the IDs and the pointer to the shared memory region in the corresponding parameters */
    init(shmid,msqid,sharedMemPtr);
}
 

/**
 * The main loop
 * @param fileName - the name of the file received from the sender.
 * @return - the number of bytes received
 */
unsigned long mainLoop(const char* fileName)
{
	/* The size of the message received from the sender */
	int msgSize = -1;
	
	/* The number of bytes received */
	int numBytesRecv = 0;
	
	/* The string representing the file name received from the sender */
	string recvFileNameStr = fileName;
	
	/* TODO: append __recv to the end of file name */
	recvFileNameStr.append("__recv");
	
	/* Open the file for writing */
	FILE* fp = fopen(recvFileNameStr.c_str(), "w");
			
	/* Error checks */
	if(!fp)
	{
		perror("fopen");	
		exit(-1);
	}
		

	/* Keep receiving until the sender sets the size to 0, indicating that
 	 * there is no more data to send.
 	 */	
	while(msgSize != 0)
	{	

		/* TODO: Receive the message and get the value of the size field. The message will be of 
		 * of type SENDER_DATA_TYPE. That is, a message that is an instance of the message struct with 
		 * mtype field set to SENDER_DATA_TYPE (the macro SENDER_DATA_TYPE is defined in 
		 * msg.h).  If the size field of the message is not 0, then we copy that many bytes from 
		 * the shared memory segment to the file. Otherwise, if 0, then we close the file 
		 * and exit.
		 *
		 * NOTE: the received file will always be saved into the file called
		 * <ORIGINAL FILENAME__recv>. For example, if the name of the original
		 * file is song.mp3, the name of the received file is going to be song.mp3__recv.
		 */
		
		/* If the sender is not telling us that we are done, then get to work */
		if(msgSize != 0)
		{
			/* TODO: count the number of bytes received */
			
			/* Save the shared memory to file */
			if(fwrite(sharedMemPtr, sizeof(char), msgSize, fp) == -1)
			{
				perror("fwrite");
			}
			
			/* DONE: Tell the sender that we are ready for the next set of bytes.
 			 * I.e., send a message of type RECV_DONE_TYPE. That is, a message
			 * of type ackMessage with mtype field set to RECV_DONE_TYPE. 
 			 */
            
            struct message sendMessage;
            sendMessage.mtype = RECV_DONE_TYPE;
            msgsnd(msqid,&sendMessage.mtype,msgSize,0);
            
            
		}
		/* We are done */
		else
		{
			/* Close the file */
			fclose(fp);
		}
	}
	
	return numBytesRecv;
}



/**
 * Performs cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */
void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
    
    //Taken from slide 62:
    cout<<"Commencing Clean Up."<<endl;
	/* DONE: Detach from shared memory */
    shmdt(sharedMemPtr);
	/* DONE: Deallocate the shared memory segment */
    shmctl(shmid,IPC_RMID,NULL);
	/* DONE: Deallocate the message queue */
    shmctl(msqid,IPC_RMID,NULL);
}

/**
 * Handles the exit signal
 * @param signal - the signal type
 */
void ctrlCSignal(int signal)
{
    /* Free system V resources */
    cout<<endl<<"Ctrl-C pressed, cleaning up and exiting."<<endl;
    cleanUp(shmid, msqid, sharedMemPtr);
    exit(0);

}

int main(int argc, char** argv)
{
	
	/* DONE: Install a signal handler (see signaldemo.cpp sample file).
 	 * If user presses Ctrl-c, your program should delete the message
 	 * queue and the shared memory segment before exiting. You may add 
	 * the cleaning functionality in ctrlCSignal().
 	 */
    signal(SIGINT, ctrlCSignal);

	/* Initialize */
	init(shmid, msqid, sharedMemPtr);
	
	/* Receive the file name from the sender */
	string fileName = recvFileName();
    
        //Uncomment the following line to test signal
        //pause();
	
	/* Go to the main loop */
	fprintf(stderr, "The number of bytes received is: %lu\n", mainLoop(fileName.c_str()));

	/* DONE: Detach from shared memory segment, and deallocate shared memory 
	 * and message queue (i.e. call cleanup) 
	 */
    cleanUp(shmid,msqid,sharedMemPtr);
		
	return 0;
}
