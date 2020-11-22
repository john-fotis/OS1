#include <iostream>
#include <string.h>
#include <openssl/md5.h>
#include "encoder.h"
using namespace std;

void calculateMD5(const char * text, char * checksum){
  static const char hexDigits[17] = "0123456789ABCDEF";
  unsigned char digest[MD5_DIGEST_LENGTH];
  char digest_str[2*MD5_DIGEST_LENGTH+1];
  int i;

  // Count digest
  MD5( (const unsigned char*)text, sizeof(text), digest );

  // Convert the hash into a hex string form
  for(i = 0; i < MD5_DIGEST_LENGTH; i++){
    digest_str[i*2]   = hexDigits[(digest[i] >> 4) & 0xF];
    digest_str[i*2+1] = hexDigits[digest[i] & 0xF];
  }
  digest_str[MD5_DIGEST_LENGTH*2] = '\0';
  strcpy(checksum, digest_str);
}

Encoder::Encoder(){
  m = new message;
  m->text = new char;
  m->checksum = new char;
  // Assume succesfull delivery on 1st try
  m->succesfull = true;
}

Encoder::~Encoder(){
  delete [] m->checksum;
  delete [] m->text;
  m = NULL;
}

void Encoder::receiveMessage(const char &txt){
  delete [] m->checksum;
  delete [] m->text;
  m->text = new char[strlen(&txt)+1];
  m->checksum = new char[2*MD5_DIGEST_LENGTH+1];
  strcpy(m->text, &txt);

  calculateMD5(m->text, m->checksum);
}

bool Encoder::decodeMessage(const message &msg){
  char newChecksum[2*MD5_DIGEST_LENGTH+1];
  delete [] m->checksum;
  delete [] m->text;
  m->text = new char[strlen(msg.text) + 1];
  m->checksum = new char[strlen(msg.checksum) + 1];
  strcpy(m->text, msg.text);
  strcpy(m->checksum, msg.checksum);
  m->succesfull = msg.succesfull;
  
  calculateMD5(m->text, newChecksum);
  cout << m->text << endl;
  cout << newChecksum << endl;
  cout << ">>>>>>> " << strcmp(m->checksum, newChecksum) << endl;

  if(strcmp(m->checksum, newChecksum) == 0)
    return true;

  // If checksums don't match request retransmition
  setMessageFaulty(*m);
  return false;
}

void Encoder::setMessageFaulty(message &m){
  m.succesfull = false;
}

message * Encoder::getMessage(){
  return m;
}

char * Encoder::deliveryMessage(){
  return m->text;
}