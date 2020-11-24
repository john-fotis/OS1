#ifndef __SHARED__MEMORY
#define __SHARED__MEMORY

#include <openssl/md5.h>

#define BLOCK_SIZE 4096
#define IPC_ERROR (-1)

typedef struct {
  char text[BLOCK_SIZE - 2*MD5_DIGEST_LENGTH - sizeof(int)];
  char checksum[2*MD5_DIGEST_LENGTH];
  bool succesfull;
} message;

message * attachBlock(char * fileName, int size, unsigned int id);
bool detachBlock(message * block);
bool destroyBlock(char * block, int size, unsigned int id);

#define P1_ENC1_BLOCK (char *)    "p1.cc"
#define ENC1_CHAN_BLOCK (char *)  "encoder1.cc"
#define CHAN_ENC2_BLOCK (char *)  "channel.cc"
#define ENC2_P2_BLOCK (char *)    "encoder2.cc"

// Define semaphores
#define SEM_PRODUCER  "/p1"
#define SEM_ENCODER1  "/enc1"
#define SEM_CHANNEL   "/chan"
#define SEM_ENCODER2  "/enc2"
#define SEM_CONSUMER  "/p2"
#define P1_ENC1_BLOCK_ID    1
#define ENC1_CHAN_BLOCK_ID  2
#define CHAN_ENC2_BLOCK_ID  3
#define ENC2_P1_BLOCK_ID    4

#endif