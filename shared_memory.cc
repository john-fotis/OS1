#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shared_memory.h"

static int getSharedBlock(char * fileName, int size) {
  key_t key;

  // Request a key
  key = ftok(fileName, 0);
  // And link it to a filename
  if (key == IPC_ERROR) {
    return IPC_ERROR;
  }
  
  // Create block or get it if it exists
  return shmget(key, size, 0644 | IPC_CREAT);
}

char * attachBlock(char * fileName, int size) {
  char * result;
  // Request the shared block id
  int sharedBlockId = getSharedBlock(fileName, size);
  if (sharedBlockId == IPC_ERROR)
    return NULL;
  
  /* 
  Map the shared block to the current proccess
  and return a pointer to it
  */
  result = (char *)shmat(sharedBlockId, NULL, 0);
  if (result == (char*)IPC_ERROR) {
    return NULL;
  }
  
  return result;
}

bool detachBlock(char * block) {
  return (shmdt(block) != IPC_ERROR);
}

bool destroyBlock(char * fileName) {
  int sharedBlockId = getSharedBlock(fileName, 0);

  if (sharedBlockId == IPC_ERROR) {
    return NULL;
  }

    return(shmctl(sharedBlockId, IPC_RMID, NULL) != IPC_ERROR);
}