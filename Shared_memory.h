#ifndef __SHARED__MEMORY
#define __SHARED__MEMORY

#include <openssl/md5.h>

#define BLOCK_SIZE 4096
#define IPC_ERROR (-1)

typedef struct {
  char text[BLOCK_SIZE - 2*MD5_DIGEST_LENGTH - sizeof(int)];
  char checksum[2*MD5_DIGEST_LENGTH];
  int status;
  // Status meaning:
  // -1 -> faulty message
  //  0 -> regular message / vulnerable to noise effect
  //  1 -> protected message / invulnerable to noise effect 
} message;

message * attachBlock(char * fileName, int size, unsigned int proj_id);
bool * attachFlagBlock(char * filename, int size, unsigned int proj_id);
bool detachBlock(message * block);
bool detachBlock(bool * block);
bool destroyBlock(char * blockName, int size, unsigned int proj_id);
bool destroyBlock(char * blockName, unsigned int proj_id);

// Filename of shared memory blocks
#define FILENAME (char *) "p1.cc"
// IDs of shared memory blocks
#define P1_ENC1_BLOCK_ID    1
#define ENC1_CHAN_BLOCK_ID  2
#define CHAN_ENC2_BLOCK_ID  3
#define ENC2_P2_BLOCK_ID    4
#define DIRECTION_BLOCK_ID  5
// Semaphore names
#define SEM_PRODUCER  "/p1"
#define SEM_ENCODER1  "/enc1"
#define SEM_CHANNEL   "/chan"
#define SEM_ENCODER2  "/enc2"
#define SEM_CONSUMER  "/p2"

#endif