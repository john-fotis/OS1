#include <iostream>
#include <string.h>

#include "Encoder.h"

void calculateMD5(const char * text, char * checksum){
  static const char hexDigits[17] = "0123456789ABCDEF";
  unsigned char digest[MD5_DIGEST_LENGTH];
  char digest_str[2*MD5_DIGEST_LENGTH];
  int i;

  // Count digest
  MD5((const unsigned char*)text, sizeof(text), digest);

  // Convert the hash into a hex string form
  for(i = 0; i < MD5_DIGEST_LENGTH; i++){
    digest_str[i*2] = hexDigits[(digest[i] >> 4) & 0xF];
    digest_str[i*2+1] = hexDigits[digest[i] & 0xF];
  }
  digest_str[MD5_DIGEST_LENGTH*2] = '\0';
  sprintf(checksum, digest_str, "%s");
}

Encoder::Encoder(){
  m = new message;
  sprintf(m->text, "%s", "\0");
  sprintf(m->checksum, "%s", "\0");
  // Assume succesfull delivery on 1st try
  m->status = 0;
}

Encoder::~Encoder(){
  m = NULL;
}

void Encoder::receiveMessage(const message &msg) {
  sprintf(m->text, msg.text, "%s");
  calculateMD5(m->text, m->checksum);
  m->status = msg.status;
}

bool Encoder::decodeMessage(const message &msg){
  char newChecksum[2*MD5_DIGEST_LENGTH];

  sprintf(m->text, msg.text, "%s");
  sprintf(m->checksum, msg.checksum, "%s");
  m->status = msg.status;
  
  calculateMD5(m->text, newChecksum);

  if(!strncmp(m->checksum, newChecksum, 2*MD5_DIGEST_LENGTH))
    return true;

  return false;
}

message * Encoder::getMessage(){
  return m;
}

char * Encoder::deliveryMessage(){
  return m->text;
}