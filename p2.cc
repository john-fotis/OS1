#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>

#include "shared_memory.h"

using namespace std;

int main(int argc, char *argv[]) {
  // Checking input validity
  if (argc != 1) {
    cout << "Correct usage: $./p2.ex //No arguments" << endl;
    return -1;
  }

  // Set up semaphores
  sem_t *sem_producer;
  sem_t *sem_consumer;
  sem_unlink(SEM_PRODUCER);
  sem_unlink(SEM_CONSUMER);

  if ((sem_producer = sem_open(SEM_PRODUCER, O_CREAT, 0660, 0)) == SEM_FAILED) {
    perror("sem_open/producer");
    exit(EXIT_FAILURE);
  }

  if ((sem_consumer = sem_open(SEM_CONSUMER, O_CREAT, 0660, 1)) == SEM_FAILED) {
    perror("sem_open/consumer");
    exit(EXIT_FAILURE);
  }

  // Get the memory block
  char *block = attachBlock(FILENAME, BLOCK_SIZE);
  if (block == NULL) {
    cout << "ERROR: Couldn't get block." << endl;
    return -1;
  }

  while (true) {
    sem_wait(sem_producer);
    if (strlen(block) > 0) {
      cout << "Reading: [" << block << "]" << endl;
      bool finished = (strcmp(block, "TERM") == 0);
      block[0] = 0;
      if (finished) {
        break;
      }
    }
    sem_post(sem_consumer);
  }

  sem_close(sem_consumer);
  sem_close(sem_producer);
  detachBlock(block);

  return 0;
}