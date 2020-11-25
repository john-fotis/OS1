#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <errno.h>

#include "Shared_memory.h"

using namespace std;

int main(int argc, char *argv[]) {
  // Checking input validity
  if (argc != 1) {
    cout << "Correct usage: $./p2.ex //No arguments" << endl;
    return -1;
  }

  // Set up semaphores - They already exist
  sem_t *sem_encoder2;
  sem_t *sem_consumer;

  // Encoder 2 semaphore
  if ((sem_encoder2 = sem_open(SEM_ENCODER2, 0)) == SEM_FAILED) {
    perror("sem_open/encoder2");
    exit(EXIT_FAILURE);
  }

  // P2 semaphore
  if ((sem_consumer = sem_open(SEM_CONSUMER, 0)) == SEM_FAILED) {
    perror("sem_open/consumer");
    exit(EXIT_FAILURE);
  }

  // Get the memory block
  message * block = attachBlock(ENC2_P2_BLOCK, BLOCK_SIZE, ENC2_P2_BLOCK_ID);
  if (block == NULL) {
    cout << "ERROR: Couldn't get block." << endl;
    return -1;
  }

  // Critical section
  do {
    sem_wait(sem_encoder2);

    if (strlen(block->text) > 0) {
      cout << "Reading: [" << block->text << "]" << endl;
    } else {
      cout << "No message found" << endl;
    }

    sem_post(sem_consumer);
  } while (strcmp(block->text, "TERM"));

  // Release semaphores and memory
  sem_close(sem_encoder2);
  sem_close(sem_consumer);
  detachBlock(block);

  // Delete the shared memory after it's no longer used
  if (destroyBlock(ENC2_P2_BLOCK, BLOCK_SIZE, ENC2_P2_BLOCK_ID)) {
    cout << "Destroyed block [" << ENC2_P2_BLOCK << "]" << endl;
  } else {
    cout << "Couldn't destroy block [" << ENC2_P2_BLOCK << "]" << endl;
    return -1;
  }

  return 0;
}