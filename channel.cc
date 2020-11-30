#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include <time.h>

#include "Channel.h"

int main(int argc, char *argv[]) {
  srand(time(NULL));
  float noise;
  // Checking input validity
  if (argc == 1) {
    noise = 0.0;
  } else if (argc == 2) {
    noise = atof(argv[1]);
  } else {
    std::cout << "Correct usage: $./channel.ex (DECIMAL {0-1})" << std::endl;
    return -1;
  }

  Channel c(noise);
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
      // Flow left <- right
      std::cout << "Channel from Encoder 2: [" << blockRight->text << "]" << std::endl;
      // Pass the message from the right side to the left
      c.receiveMessage(*blockRight);
    }
    // And retrieve it from the other end
    m = c.transferMessage();

    switch(m->status) {
      case -1:
        // This message is corrupted - push it back
        if (!*directionFlag) {
          // memcpy(blockLeft, m, sizeof(*blockLeft));
          blockLeft->status = -1;
          std::cout << "Pushing back to Encoder 1..." << std::endl;
          // Signal encoder 1
          sem_post(sem_encoder1);
        } else {
          // memcpy(blockRight, m, sizeof(*blockRight));
          blockRight->status = -1;
          std::cout << "Pushing back to Encoder 2..." << std::endl;
          // Signal encoder 2
          sem_post(sem_encoder2);
        }
        break;
      case 0:
        // Unprotected message or 1st try - regular transmition
      case 1:
        // Protected message, control message or 2nd try - Transmit safely
        if (*directionFlag) {
          memcpy(blockRight, m, sizeof(*blockRight));
          std::cout << "Channel to Encoder 2: [" << blockRight->text << "]" << std::endl;
          // Signal encoder 2
          sem_post(sem_encoder2);
        } else {
          memcpy(blockLeft, m, sizeof(*blockLeft));
          std::cout << "Channel to Encoder 1: [" << blockLeft->text << "]" << std::endl;
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
  detachBlock(directionFlag);

  // Delete the shared memory after it's no longer used
  if (destroyBlock(FILENAME, sizeof(bool *), ENC1_CHAN_BLOCK_ID)) {
    std::cout << "Destroyed block [" << ENC1_CHAN_BLOCK_ID << "]" << std::endl;
  } else {
    std::cout << "Couldn't destroy block [" << ENC1_CHAN_BLOCK_ID << "]" << std::endl;
    return -1;
  }

  return 0;
}