#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include <time.h>

#include "Channel.h"

int main(int argc, char *argv[]) {
  srand(time(NULL));

  // Checking input validity
  if (argc != 2) {
    std::cout << "Correct usage: $./channel.ex [DECIMAL {0-1}]" << std::endl;
    return -1;
  }

  Channel c(atof(argv[1]));
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
  // Left channel block is shared with encoder 1
  message *blockLeft = attachBlock(FILENAME, BLOCK_SIZE, ENC1_CHAN_BLOCK_ID);
  if (blockLeft == NULL) {
    std::cout << "ERROR: Couldn't get block." << std::endl;
    return -1;
  }

  // Right channel block is shared with encoder 2
  message *blockRight = attachBlock(FILENAME, BLOCK_SIZE, CHAN_ENC2_BLOCK_ID);
  if (blockRight == NULL) {
    std::cout << "ERROR: Couldn't get block." << std::endl;
    return -1;
  }

  // This shared memory block contains the flag that indicates the flow of the message
  bool *directionFlag = attachFlagBlock(FILENAME, sizeof(bool *), DIRECTION_BLOCK_ID);
  if (directionFlag == NULL) {
    std::cout << "ERROR: Couldn't get flag block." << std::endl;
    return -1;
  }

  // Critical section
  do {
    sem_wait(sem_channel);

    if (*directionFlag) {
      // Flow left -> right
      std::cout << "Channel from Encoder 1: [" << blockLeft->text << "]" << std::endl;
      // Pass the message from the left side to the right
      c.receiveMessage(*blockLeft);
    } else {
      std::cout << "Channel from Encoder 2: [" << blockRight->text << "]" << std::endl;
      // Pass the message from the right side to the left
      c.receiveMessage(*blockRight);
    }
    // And retrieve it from the other end
    m = c.transferMessage();

    switch(m->status) {
      case -1:
        // This message is corrupted - push it back to enc1
        sprintf(blockLeft->text, blockRight->text, "%s");
        sprintf(blockLeft->checksum, blockRight->checksum, "%s");
        blockLeft->status = blockRight->status;
        std::cout << "Requesting retransmition from Encoder 1..." << std::endl;
        // Signal encoder 1
        sem_post(sem_encoder1);
        break;
      case 0:
        // Unprotected message - 1st try -> send it to enc2
      case 1:
        // Protected message - Control message or 2nd try
        if (*directionFlag) {
          sprintf(blockRight->text, m->text, "%s");
          sprintf(blockRight->checksum, m->checksum, "%s");
          blockRight->status = m->status;
          std::cout << "Channel to Encoder 2: [" << blockRight->text << "]" << std::endl;
          // Signal encoder 2
          sem_post(sem_encoder2);
        } else {
          sprintf(blockLeft->text, m->text, "%s");
          sprintf(blockLeft->checksum, m->checksum, "%s");
          blockLeft->status = m->status;
          std::cout << "Channel to Encoder 2: [" << blockLeft->text << "]" << std::endl;
          // Signal encoder 1
          sem_post(sem_encoder1);
        }
        break;
      default:
        break;
    }
  } while (strcmp(blockRight->text, "TERM"));

  // Release semaphores and memory
  sem_close(sem_encoder1);
  sem_close(sem_channel);
  sem_close(sem_encoder2);
  detachBlock(blockLeft);
  detachBlock(blockRight);
  detachFlagBlock(directionFlag);

  // Delete the shared memory after it's no longer used
  if (destroyBlock(FILENAME, BLOCK_SIZE, ENC1_CHAN_BLOCK_ID)) {
    std::cout << "Destroyed block [" << ENC1_CHAN_BLOCK_ID << "]" << std::endl;
  } else {
    std::cout << "Couldn't destroy block [" << ENC1_CHAN_BLOCK_ID << "]" << std::endl;
    return -1;
  }

  return 0;
}