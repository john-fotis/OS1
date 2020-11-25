#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <errno.h>

#include "Channel.h"

using namespace std;

int main(int argc, char *argv[]) {
  // Checking input validity
  if (argc != 1) {
    cout << "Correct usage: $./p2.ex //No arguments" << endl;
    return -1;
  }

  message * m;
  Channel c;

  // Set up semaphores - They already exist
  sem_t *sem_producer;
  sem_t *sem_encoder1;
  sem_t *sem_channel;

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

  // Channel semaphore
  if ((sem_channel = sem_open(SEM_CHANNEL, 0)) == SEM_FAILED) {
    perror("sem_open/channel");
    exit(EXIT_FAILURE);
  }

  // Left channel block is shared with encoder 1
  message *blockLeft = attachBlock(ENC1_CHAN_BLOCK, BLOCK_SIZE, ENC1_CHAN_BLOCK_ID);
  if (blockLeft == NULL) {
    cout << "ERROR: Couldn't get block." << endl;
    return -1;
  }

  // Right channel block is shared with encoder 2
  message *blockRight = attachBlock(CHAN_ENC2_BLOCK, BLOCK_SIZE, CHAN_ENC2_BLOCK_ID);
  if (blockRight == NULL) {
    cout << "ERROR: Couldn't get block." << endl;
    return -1;
  }

  // Critical section
  do {
    sem_wait(sem_encoder1);

    cout << "Channel from Encoder1: [" << blockLeft->text << "]" << endl;
    c.receiveMessage(*blockLeft);
    
    m = c.transferMessage();
    strncpy(blockRight->text, m->text, sizeof(m->text));
    strncpy(blockRight->checksum, m->checksum, sizeof(m->checksum));
    blockRight->succesfull = m->succesfull;
    cout << "Channel to encoder 2: [" << blockRight->text << "]" << endl;
    
    sem_post(sem_channel);
  } while (strcmp(blockRight->text, "TERM"));

  // Release semaphores and memory
  sem_close(sem_producer);
  sem_close(sem_encoder1);
  sem_close(sem_channel);
  detachBlock(blockLeft);
  detachBlock(blockRight);

  // Delete the shared memory after it's no longer used
  if (destroyBlock(ENC1_CHAN_BLOCK, BLOCK_SIZE, ENC1_CHAN_BLOCK_ID)) {
    cout << "Destroyed block [" << ENC1_CHAN_BLOCK << "]" << endl;
  } else {
    cout << "Couldn't destroy block [" << ENC1_CHAN_BLOCK << "]" << endl;
    return -1;
  }

  return 0;
}