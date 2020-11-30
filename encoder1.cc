#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <errno.h>

#include "Encoder.h"

int main(int argc, char *argv[]) {
  // Checking input validity
  if (argc != 1) {
    std::cout << "Correct usage: $./encoder1.ex //No arguments" << std::endl;
    return -1;
  }

  Encoder enc1;
  message * m;

  // Set up semaphores - They already exist
  sem_t *sem_channel;
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

  // Channel semaphore
  if ((sem_channel = sem_open(SEM_CHANNEL, 0)) == SEM_FAILED) {
    perror("sem_open/channel");
    exit(EXIT_FAILURE);
  }

  // Left encoder 1 block is shared with p1
  message *blockLeft = attachBlock(FILENAME, BLOCK_SIZE, P1_ENC1_BLOCK_ID);
  if (blockLeft == NULL) {
    std::cout << "ERROR: Couldn't get block." << std::endl;
    return -1;
  }

  // Right encoder 1 block is shared with channel
  message *blockRight = attachBlock(FILENAME, BLOCK_SIZE, ENC1_CHAN_BLOCK_ID);
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
    sem_wait(sem_encoder1);

    if (*directionFlag) {
      // Normal operation p1 -> p2
      std::cout << "Encoder 1 from P1: [" << blockLeft->text << "]" << std::endl;
      enc1.encodeMessage(*blockLeft);
      m = enc1.getMessage();
      // Just pass the message to channel
      memcpy(blockRight, m, sizeof(*blockRight));
      std::cout << "Encoder 1 to Channel: [" << blockRight->text << "]" << std::endl;
      // Signal channel
      sem_post(sem_channel);
    } else {
      // Flow: left <- right
      std::cout << "Encoder 1 from Channel: [" << blockRight->text << "]" << std::endl;
      if (blockRight->status == -1) {
        // Channel is requesting restransmition of a message
        blockLeft->status = -1;
        // Signal p1 to resend
        sem_post(sem_producer);
      } else {
        // This is a message from p2
        if (enc1.decodeMessage(*blockRight)) {
          // Message decoded successfully
          memcpy(blockLeft, blockRight, sizeof(*blockRight));

          std::cout << "Encoder 1 to P1: [" << blockLeft->text << "]" << std::endl;
          // Signal p1
          sem_post(sem_producer);
        } else {
          std::cout << "Message corrupted - Requesting retransmition..." << std::endl;
          // Update the message status
          blockRight->status = -1;
          // Revert the flow of transmition
          *directionFlag = true;
          // Signal channel
          sem_post(sem_channel);
        }
      }
    }
  } while (strcmp(blockRight->text, "TERM"));

  // Release semaphores and memory
  sem_close(sem_producer);
  sem_close(sem_encoder1);
  sem_close(sem_channel);
  detachBlock(blockLeft);
  detachBlock(blockRight);
  detachBlock(directionFlag);

  // Delete the shared memory after it's no longer used
  if (destroyBlock(FILENAME, sizeof(bool *), P1_ENC1_BLOCK_ID)) {
    std::cout << "Destroyed block [" << P1_ENC1_BLOCK_ID << "]" << std::endl;
  } else {
    std::cout << "Couldn't destroy block [" << P1_ENC1_BLOCK_ID << "]" << std::endl;
    return -1;
  }

  return 0;
}