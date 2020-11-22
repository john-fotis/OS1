#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <errno.h>

#include "shared_memory.h"

using namespace std;

int main(int argc, char * argv[]) {
  // Checking input validity
  if (argc != 2) {
    cout << "Correct usage: $./p1.ex [YOUR_MESSAGE_HERE]" << endl;
    return -1;
  }

  // Set up semaphores that ALREADY exist
  sem_t * sem_producer;
  sem_t * sem_consumer;

  if ((sem_producer = sem_open(SEM_PRODUCER, 0)) == SEM_FAILED) {
    perror("sem_open/producer");
    exit(EXIT_FAILURE);
  }

  if ((sem_consumer = sem_open(SEM_CONSUMER, 0)) == SEM_FAILED) {
    perror("sem_open/consumer");
    exit(EXIT_FAILURE);
  }

  // Get the memory block
  char * block = attachBlock(FILENAME, BLOCK_SIZE);
  if (block == NULL) {
    cout << "ERROR: Couldn't get block." << endl;
    return -1;
  }

  for (int i=0; i<10; i++) {
    sem_wait(sem_consumer);
    cout << "Writing [" << argv[1] << "] now..." << endl;
    strncpy(block, argv[1], BLOCK_SIZE);
    strncpy(block, argv[1], BLOCK_SIZE);
    sem_post(sem_producer);
  }

  sem_close(sem_producer);
  sem_close(sem_consumer);
  detachBlock(block);

  return 0;
}