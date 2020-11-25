#include <iostream>
#include <string.h>

#include "Channel.h"

using namespace std;

Channel::Channel(float ratio){
  errorRatio = ratio;
  m = new message;
  strcpy(m->text, "\0");
  strcpy(m->checksum, "\0");
  m->succesfull = 1;
}

Channel::~Channel(){
  m = NULL;
}

void Channel::receiveMessage(const message &msg){
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

message * Channel::transferMessage(){
  return m;
}