#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <errno.h>

#include "Encoder.h"

int main(int argc, char *argv[]) {
  // Checking input validity
  if (argc != 1) {
    std::cout << "Correct usage: $./encoder2.ex //No arguments" << std::endl;
    return -1;
  }

  Encoder enc2;
  message * m;

  // Set up semaphores - They already exist
  sem_t *sem_channel;
  sem_t *sem_encoder2;
  sem_t *sem_consumer;

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

  // P2 semaphore
  if ((sem_consumer = sem_open(SEM_CONSUMER, 0)) == SEM_FAILED) {
    perror("sem_open/consumer");
    exit(EXIT_FAILURE);
  }

  // Left encoder 2 block is shared with channel
  message *blockLeft = attachBlock(FILENAME, BLOCK_SIZE, CHAN_ENC2_BLOCK_ID);
  if (blockLeft == NULL) {
    std::cout << "ERROR: Couldn't get block." << std::endl;
    return -1;
  }

  // Right encoder 2 block is shared with p2
  message *blockRight = attachBlock(FILENAME, BLOCK_SIZE, ENC2_P2_BLOCK_ID);
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

  // Critical Section
  do {
    sem_wait(sem_encoder2);

    if (*directionFlag) {
      // Flow left: -> right
      std::cout << "Encoder 2 from Channel: [" << blockLeft->text << "]" << std::endl;
      if (blockLeft->status == -1) {
        // Channel is requesting restransmition of a message
        blockRight->status = -1;
        // Signal p2 to resend
        sem_post(sem_consumer);
      } else {
        // This is a message from p1
        if (enc2.decodeMessage(*blockLeft)) {
          // Message decoded successfully
          memcpy(blockRight, blockLeft, sizeof(*blockLeft));

          std::cout << "Encoder 2 to P2: [" << blockRight->text << "]" << std::endl;
          // Signal p2
          sem_post(sem_consumer);
        } else {
          std::cout << "Message corrupted - Requesting retransmition..." << std::endl;
          // Update the message status
          blockLeft->status = -1;
          // Revert the flow of transmition
          *directionFlag = false;
          // Signal channel
          sem_post(sem_channel);
        }
      }
    } else {
      // Normal operation p2 -> p1
      std::cout << "Encoder 2 from P2: [" << blockRight->text << "]" << std::endl;
      enc2.encodeMessage(*blockRight);
      m = enc2.getMessage();
      // Just pass the message to channel
      memcpy(blockLeft, m, sizeof(*blockLeft));
      std::cout << "Encoder 2 to Channel: [" << blockLeft->text << "]" << std::endl;
      // Signal channel
      sem_post(sem_channel);
    }
  } while (strcmp(blockRight->text, "TERM"));

  // Release semaphores and memory
  sem_close(sem_channel);
  sem_close(sem_encoder2);
  sem_close(sem_consumer);
  detachBlock(blockLeft);
  detachBlock(blockRight);
  detachBlock(directionFlag);

  // Delete the shared memory after it's no longer used
  if (destroyBlock(FILENAME, sizeof(bool *), CHAN_ENC2_BLOCK_ID)) {
    std::cout << "Destroyed block [" << CHAN_ENC2_BLOCK_ID << "]" << std::endl;
  } else {
    std::cout << "Couldn't destroy block [" << CHAN_ENC2_BLOCK_ID << "]" << std::endl;
    return -1;
  }

  return 0;
}