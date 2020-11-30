#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <errno.h>

#include "Shared_memory.h"

int main(int argc, char *argv[]) {
  // Checking input validity
  if (argc != 1) {
    std::cout << "Correct usage: $./p2.ex //No arguments" << std::endl;
    return -1;
  }

  char buffer[BLOCK_SIZE - 2 * MD5_DIGEST_LENGTH - sizeof(int)];

  // Set up semaphores - They already exist
  sem_t *sem_encoder2;
  sem_t *sem_consumer;

  // P2 semaphore
  if ((sem_consumer = sem_open(SEM_CONSUMER, 0)) == SEM_FAILED) {
    perror("sem_open/consumer");
    exit(EXIT_FAILURE);
  }

  // Encoder 2 semaphore
  if ((sem_encoder2 = sem_open(SEM_ENCODER2, 0)) == SEM_FAILED) {
    perror("sem_open/encoder2");
    exit(EXIT_FAILURE);
  }

  // Get the memory block
  message * block = attachBlock(FILENAME, BLOCK_SIZE, ENC2_P2_BLOCK_ID);
  if (block == NULL) {
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
    sem_wait(sem_consumer);

    if ((*directionFlag && !strcmp(block->text, "TERM"))) {
      // If you got a "TERM" message from P1:
      break;
    }

    if (block->status == -1) {
      // Encoder 2 is requesting retransmition
      block->status = 1;
      std::cout << "I had to rewrite the message: [" << block->text << "]" << std::endl;
    } else {
      // Received message from p1
      std::cout << "Reading: [" << block->text << "]" << std::endl;
      // Write a reply
      std::cout << "Write a response:" << std::endl;
      std::cin.getline(buffer, sizeof(buffer));
      if (strcmp(buffer, "TERM")) {
        // This is a regular message
        std::cout << "Replying [" << buffer << "] now..." << std::endl;
        block->status = 0;
      } else {
        // This is a control message - Transmit safely
        std::cout << "Giving order to stop now..." << std::endl;
        block->status = 1;
      }
    }

    // Revert the flow
    *directionFlag = false;

    sprintf(block->text, "%s", buffer);
    sprintf(block->checksum, "%s", "\0");

    // Signal encoder 2
    sem_post(sem_encoder2);

  } while (strcmp(block->text, "TERM"));

  // Release semaphores and memory
  sem_close(sem_encoder2);
  sem_close(sem_consumer);
  detachBlock(block);
  detachBlock(directionFlag);

  // Delete the shared memory after it's no longer used
  if (destroyBlock(FILENAME, BLOCK_SIZE, ENC2_P2_BLOCK_ID)) {
    std::cout << "Destroyed block [" << ENC2_P2_BLOCK_ID << "]" << std::endl;
  } else {
    std::cout << "Couldn't destroy block [" << ENC2_P2_BLOCK_ID << "]" << std::endl;
    return -1;
  }

  // Delete the flag shared memory
  if (destroyBlock(FILENAME, DIRECTION_BLOCK_ID)) {
    std::cout << "Destroyed block [" << DIRECTION_BLOCK_ID << "]" << std::endl;
  } else {
    std::cout << "Couldn't destroy block [" << DIRECTION_BLOCK_ID << "]" << std::endl;
    return -1;
  }

  return 0;
}