#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>

#include "shared_memory.h"
#include "P.h"
// #include "encoder.h"

using namespace std;

int main(int argc, char * argv[]) {
  // Usefull data takes up to 75% of the block
  char buffer[BLOCK_SIZE/2+BLOCK_SIZE/4];

  // message m;
  P p1;

  // Checking input validity
  if (argc != 1) {
    cout << "Correct usage: $./p1.ex //No arguments" << endl;
    return -1;
  }

  // Set up semaphores that ALREADY exist
  sem_t * sem_producer;
  sem_t * sem_encoder1;
  sem_t * sem_channel;
  sem_t * sem_encoder2;
  sem_t * sem_consumer;
  sem_unlink(SEM_PRODUCER);
  sem_unlink(SEM_ENCODER1);
  sem_unlink(SEM_CHANNEL);
  sem_unlink(SEM_ENCODER2);
  sem_unlink(SEM_CONSUMER);

  if ((sem_producer = sem_open(SEM_PRODUCER, O_CREAT, 0660, 0)) == SEM_FAILED) {
    perror("sem_open/producer");
    exit(EXIT_FAILURE);
  }

  if ((sem_encoder1 = sem_open(SEM_PRODUCER, O_CREAT, 0660, 0)) == SEM_FAILED) {
    perror("sem_open/encoder1");
    exit(EXIT_FAILURE);
  }

  if ((sem_channel = sem_open(SEM_PRODUCER, O_CREAT, 0660, 0)) == SEM_FAILED) {
    perror("sem_open/channe;");
    exit(EXIT_FAILURE);
  }

  if ((sem_encoder2 = sem_open(SEM_PRODUCER, O_CREAT, 0660, 0)) == SEM_FAILED) {
    perror("sem_open/encoder2");
    exit(EXIT_FAILURE);
  }

  if ((sem_consumer = sem_open(SEM_CONSUMER, O_CREAT, 0660, 1)) == SEM_FAILED) {
    perror("sem_open/consumer");
    exit(EXIT_FAILURE);
  }

  // Get the memory block
  message * block = attachBlock(P1_ENC1_BLOCK, BLOCK_SIZE, P1_ENC1_BLOCK_ID);
  if (block == NULL) {
    cout << "ERROR: Couldn't get block." << endl;
    return -1;
  }

  do {
    cout << block << endl;
    cout << "Enter your message:\n";
    cin.getline (buffer, sizeof(buffer));
    p1.setMessage(buffer);

    sem_wait(sem_consumer);

    block->succesfull = 1;
    strcpy(block->text, buffer);
    strcpy(block->checksum, "");

    cout << "Writing [" << block->text << "] now..." << endl;

    sem_post(sem_producer);
  } while (strcmp(block->text, "TERM"));

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

  if (destroyBlock(P1_ENC1_BLOCK, BLOCK_SIZE, P1_ENC1_BLOCK_ID)) {
    cout << "Destroyed block [" << P1_ENC1_BLOCK << "]" << endl;
  } else {
    cout << "Couldn't destroy block [" << P1_ENC1_BLOCK << "]" << endl;
    return -1;
  }

  return 0;
}