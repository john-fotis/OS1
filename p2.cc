#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <errno.h>

#include "shared_memory.h"

using namespace std;

int main(int argc, char *argv[]) {
  // message m;
  // Checking input validity
  if (argc != 1) {
    cout << "Correct usage: $./p2.ex //No arguments" << endl;
    return -1;
  }

  // Set up semaphores
  sem_t *sem_producer;
  sem_t *sem_consumer;

  if ((sem_producer = sem_open(SEM_PRODUCER, 0)) == SEM_FAILED) {
    perror("sem_open/producer");
    exit(EXIT_FAILURE);
  }

  if ((sem_consumer = sem_open(SEM_CONSUMER, 0)) == SEM_FAILED) {
    perror("sem_open/consumer");
    exit(EXIT_FAILURE);
  }

  // Get the memory block
  message * block = attachBlock(P1_ENC1_BLOCK, BLOCK_SIZE, 1);
  if (block == NULL) {
    cout << "ERROR: Couldn't get block." << endl;
    return -1;
  }

  do {
    sem_wait(sem_producer);
    if (strlen(block->text) > 0) {
      cout << "Reading: [" << block->text << "]" << endl;
    }
    sem_post(sem_consumer);
  } while (strcmp(block->text, "TERM"));

  sem_close(sem_consumer);
  sem_close(sem_producer);
  detachBlock(block);

  return 0;
}