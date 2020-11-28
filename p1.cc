#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>

#include "Shared_memory.h"

int main(int argc, char * argv[]) {
  // Checking input validity
  if (argc != 1) {
    std::cout << "Correct usage: $./p1.ex //No arguments" << std::endl;
    return -1;
  }

  char buffer[BLOCK_SIZE - 2 * MD5_DIGEST_LENGTH - sizeof(int)];

  // Set up semaphores
  sem_t *sem_producer;
  sem_t *sem_encoder1;
  sem_t *sem_channel;
  sem_t *sem_encoder2;
  sem_t *sem_consumer;

  // Unlink them if they already exist before "sem_open"
  sem_unlink(SEM_PRODUCER);
  sem_unlink(SEM_ENCODER1);
  sem_unlink(SEM_CHANNEL);
  sem_unlink(SEM_ENCODER2);
  sem_unlink(SEM_CONSUMER);

  // P1 semaphore
  if ((sem_producer = sem_open(SEM_PRODUCER, O_CREAT, 0660, 1)) == SEM_FAILED) {
    perror("sem_open/producer");
    exit(EXIT_FAILURE);
  }

  // Encoder 1 semaphore
  if ((sem_encoder1 = sem_open(SEM_ENCODER1, O_CREAT, 0660, 0)) == SEM_FAILED) {
    perror("sem_open/encoder1");
    exit(EXIT_FAILURE);
  }

  // Channel semaphore
  if ((sem_channel = sem_open(SEM_CHANNEL, O_CREAT, 0660, 0)) == SEM_FAILED) {
    perror("sem_open/channe;");
    exit(EXIT_FAILURE);
  }

  // Encoder 2 semaphore
  if ((sem_encoder2 = sem_open(SEM_ENCODER2, O_CREAT, 0660, 0)) == SEM_FAILED) {
    perror("sem_open/encoder2");
    exit(EXIT_FAILURE);
  }

  // Consumer semaphore
  if ((sem_consumer = sem_open(SEM_CONSUMER, O_CREAT, 0660, 0)) == SEM_FAILED) {
    perror("sem_open/consumer");
    exit(EXIT_FAILURE);
  }

  // Get the memory block with encoder 1
  message * block = attachBlock(FILENAME, BLOCK_SIZE, P1_ENC1_BLOCK_ID);
  if (block == NULL) {
    std::cout << "ERROR: Couldn't get block." << std::endl;
    return -1;
  }

  // This shared memory block contains the flag that indicates the flow of the message
  bool * directionFlag = attachFlagBlock(FILENAME, sizeof(bool *), DIRECTION_BLOCK_ID);
  if (directionFlag == NULL) {
    std::cout << "ERROR: Couldn't get flag block." << std::endl;
    return -1;
  }
  // Initialize the flow as normal
  *directionFlag = true;

  // Critical section
  do {
    sem_wait(sem_producer);

    if (block->status == -1) {
      // Encoder 1 is requesting retransmition
      block->status = 1;
      std::cout << "I had to rewrite the message: [" << block->text << "]" << std::endl;
    } else {
      if (!*directionFlag) {
        // Received response from p2
        std::cout << "P2's response: [" << block->text << "]" << std::endl;
      }
      if (strcmp(block->text, "TERM")) {
        // Write a new message
        std::cout << "Enter your message:" << std::endl;
        std::cin.getline(buffer, sizeof(buffer));
        if (strcmp(buffer, "TERM")) {
          // This is a regular message
          std::cout << "Writing [" << buffer << "] now..." << std::endl;
          block->status = 0;
        } else {
          // This is a control message - Transmit safely
          std::cout << "Giving order to stop now..." << std::endl;
          block->status = 1;
        }

        // Fix direction flag to normal flow
        *directionFlag = true;

        sprintf(block->text, "%s", buffer);
        sprintf(block->checksum, "%s", "\0");
        
        // Signal encoder 1
        sem_post(sem_encoder1);
      }
    }
  } while (strcmp(block->text, "TERM"));

    // Release semaphores and memory
    sem_close(sem_producer);
    sem_close(sem_encoder1);
    sem_close(sem_channel);
    sem_close(sem_encoder2);
    sem_close(sem_consumer);
    sem_unlink(SEM_PRODUCER);
    sem_unlink(SEM_ENCODER1);
    sem_unlink(SEM_CHANNEL);
    sem_unlink(SEM_ENCODER2);
    sem_unlink(SEM_CONSUMER);
    detachBlock(block);
    detachFlagBlock(directionFlag);

    return 0;
}