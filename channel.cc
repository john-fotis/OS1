#include <iostream>
#include <string.h>
#include "channel.h"
using namespace std;

Channel::Channel(float ratio){
  errorRatio = ratio;
  m = new message;
  m->text = new char;
  m->checksum = new char;
  strcpy(m->text,"");
  strcpy(m->checksum,"");
}

Channel::~Channel(){
  delete [] m->checksum;
  delete [] m->text;
  m = NULL;
}

void Channel::receiveMessage(const message &msg){
  delete [] m->text;
  delete [] m->checksum;
  m->text = new char [strlen(msg.text) + 1];
  m->checksum = new char [strlen(msg.checksum) + 1];
  strcpy (m->text, msg.text);
  strcpy (m->checksum, msg.checksum);
  m->succesfull = msg.succesfull;

  // If this is the first transmition:
  if(m->succesfull){
    double noise = (double) rand() / (RAND_MAX);
    if (noise < errorRatio){
      cout << noise << endl;
      unsigned int i = 0;
      while (i < strlen(m->text)){
        m->text[i] = '-';
        i++;
      }
    }
  }
}

message * Channel::pushBackMessage(){
  // Send it to enc1 for fix
  return m;
}

message * Channel::transmitMessage(){
  // Send it to enc2
  return m;
}