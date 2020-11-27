#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <errno.h>

#include "Shared_memory.h"

int main(int argc, char *argv[]) {
  // Checking input validity
  if (argc != 1) {
    std::cout << "Correct usage: $./p2.ex //No arguments" << std::endl;
    return -1;
  }

  // Set up semaphores - They already exist
  sem_t *sem_consumer;
  sem_t *sem_producer;

  // P2 semaphore
  if ((sem_consumer = sem_open(SEM_CONSUMER, 0)) == SEM_FAILED) {
    perror("sem_open/consumer");
    exit(EXIT_FAILURE);
  }

  // P1 semaphore
  if ((sem_producer = sem_open(SEM_PRODUCER, 0)) == SEM_FAILED) {
    perror("sem_open/producer");
    exit(EXIT_FAILURE);
  }

  // Get the memory block
  message * block = attachBlock(FILENAME, BLOCK_SIZE, ENC2_P2_BLOCK_ID);
  if (block == NULL) {
    std::cout << "ERROR: Couldn't get block." << std::endl;
    return -1;
  }

  // Critical section
  do {
    sem_wait(sem_consumer);

    if (strlen(block->text) > 0) {
      std::cout << "Reading: [" << block->text << "]" << std::endl;
    } else {
      std::cout << "No message found" << std::endl;
    }

    sem_post(sem_producer);
  } while (strcmp(block->text, "TERM"));

  // Release semaphores and memory
  sem_close(sem_producer);
  sem_close(sem_consumer);
  detachBlock(block);

  // Delete the shared memory after it's no longer used
  if (destroyBlock(FILENAME, BLOCK_SIZE, ENC2_P2_BLOCK_ID)) {
    std::cout << "Destroyed block [" << ENC2_P2_BLOCK_ID << "]" << std::endl;
  } else {
    std::cout << "Couldn't destroy block [" << ENC2_P2_BLOCK_ID << "]" << std::endl;
    return -1;
  }

  return 0;
}