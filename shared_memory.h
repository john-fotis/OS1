#ifndef __SHARED__MEMORY
#define __SHARED__MEMORY

char * attachBlock(char * fileName, int size);
bool detachBlock(char * block);
bool destroyBlock(char * block);

#define BLOCK_SIZE 4096
/*
  P1 (writer/producer) proccess
  will create the shared memory block
*/
#define FILENAME (char*) "p1.cc"
#define IPC_ERROR (-1)
// Define semaphores
#define SEM_PRODUCER "/p1"
#define SEM_CONSUMER "/p2"

#endif