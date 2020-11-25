#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <errno.h>

#include "Encoder.h"

using namespace std;

int main(int argc, char *argv[]) {
  // Checking input validity
  if (argc != 1) {
    cout << "Correct usage: $./encoder1.ex //No arguments" << endl;
    return -1;
  }

  Encoder enc1;
  message * m;

  // Set up semaphores - They already exist
  sem_t *sem_producer;
  sem_t *sem_encoder1;

  // Producer semaphore
  if ((sem_producer = sem_open(SEM_PRODUCER, 0)) == SEM_FAILED) {
    perror("sem_open/producer");
    exit(EXIT_FAILURE);
  }

  // Encoder 1 semaphore
  if ((sem_encoder1 = sem_open(SEM_ENCODER1, 0)) == SEM_FAILED) {
    perror("sem_open/encoder1");
    exit(EXIT_FAILURE);
  }

  // Left encoder 1 block is shared with p1
  message *blockLeft = attachBlock(P1_ENC1_BLOCK, BLOCK_SIZE, P1_ENC1_BLOCK_ID);
  if (blockLeft == NULL) {
    cout << "ERROR: Couldn't get block." << endl;
    return -1;
  }

  // Right encoder 1 block is shared with channel
  message *blockRight = attachBlock(ENC1_CHAN_BLOCK, BLOCK_SIZE, ENC1_CHAN_BLOCK_ID);
  if (blockRight == NULL) {
    cout << "ERROR: Couldn't get block." << endl;
    return -1;
  }

  // Critical section
  do {
    sem_wait(sem_producer);

    cout << "Encoder 1 from P1: [" << blockLeft->text << "]" << endl;
    enc1.receiveMessage(*blockLeft->text);

    m = enc1.getMessage();
    strncpy(blockRight->text, m->text, sizeof(m->text));
    strncpy(blockRight->checksum, m->checksum, sizeof(m->checksum));
    blockRight->succesfull = m->succesfull;
    cout << "Encoder 1 to Channel: [" << blockRight->text << "]" << endl;

    sem_post(sem_encoder1);
  } while (strcmp(blockRight->text, "TERM"));

  // Release semaphores and memory
  sem_close(sem_producer);
  sem_close(sem_encoder1);
  detachBlock(blockLeft);
  detachBlock(blockRight);

  // Delete the shared memory after it's no longer used
  if (destroyBlock(P1_ENC1_BLOCK, BLOCK_SIZE, P1_ENC1_BLOCK_ID)) {
    cout << "Destroyed block [" << P1_ENC1_BLOCK << "]" << endl;
  } else {
    cout << "Couldn't destroy block [" << P1_ENC1_BLOCK << "]" << endl;
    return -1;
  }

  return 0;
}