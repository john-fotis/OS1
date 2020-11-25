#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <errno.h>

#include "Shared_memory.h"
#include "Encoder.h"

using namespace std;

int main(int argc, char *argv[]) {
  // Checking input validity
  if (argc != 1) {
    cout << "Correct usage: $./encoder2.ex //No arguments" << endl;
    return -1;
  }

  Encoder enc2;
  message * m;

  // Set up semaphores - They already exist
  sem_t *sem_encoder1;
  sem_t *sem_channel;
  sem_t *sem_encoder2;

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

  // Encoder 2 semaphore
  if ((sem_encoder2 = sem_open(SEM_ENCODER2, 0)) == SEM_FAILED) {
    perror("sem_open/encoder2");
    exit(EXIT_FAILURE);
  }

  // Left encoder 2 block is shared with channel
  message *blockLeft = attachBlock(CHAN_ENC2_BLOCK, BLOCK_SIZE, CHAN_ENC2_BLOCK_ID);
  if (blockLeft == NULL) {
    cout << "ERROR: Couldn't get block." << endl;
    return -1;
  }

  // Right encoder 2 block is shared with p2
  message *blockRight = attachBlock(ENC2_P2_BLOCK, BLOCK_SIZE, ENC2_P2_BLOCK_ID);
  if (blockRight == NULL) {
    cout << "ERROR: Couldn't get block." << endl;
    return -1;
  }

  // Critical cSection
  do {
    sem_wait(sem_channel);

    cout << "Encoder 2 from Channel: [" << blockLeft->text << "]" << endl;
    if (enc2.decodeMessage(*blockLeft)) {
      m = enc2.getMessage();
      strncpy(blockRight->text, m->text, sizeof(m->text));
      strncpy(blockRight->checksum, m->checksum, sizeof(m->checksum));
      blockRight->succesfull = m->succesfull;
      cout << "Encoder 2 to P2: [" << blockRight->text << "]" << endl;
      sem_post(sem_encoder2);
    } else {
      cout << "Message corrupted - Requesting retransmition..." << endl;
      sem_post(sem_encoder1);
    }
  } while (strcmp(blockRight->text, "TERM"));

  // Release semaphores and memory
  sem_close(sem_channel);
  sem_close(sem_encoder2);
  sem_close(sem_encoder1);
  detachBlock(blockLeft);
  detachBlock(blockRight);

  // Delete the shared memory after it's no longer used
  if (destroyBlock(ENC2_P2_BLOCK, BLOCK_SIZE, ENC2_P2_BLOCK_ID)) {
    cout << "Destroyed block [" << ENC2_P2_BLOCK << "]" << endl;
  } else {
    cout << "Couldn't destroy block [" << ENC2_P2_BLOCK << "]" << endl;
    return -1;
  }

  return 0;
}