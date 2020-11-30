#include <iostream>
#include <string.h>

#include "Channel.h"

Channel::Channel(float ratio){
  errorRatio = ratio;
  m = new message;
  sprintf(m->text, "%s", "\0");
  sprintf(m->checksum, "%s", "\0");
  m->status = 0;
}

Channel::~Channel(){
  m = NULL;
}

void Channel::receiveMessage(const message &msg){
  memcpy(m, &msg, sizeof(message));

  // If this is the first transmition:
  if(!m->status){
    double noise = (double) rand() / (RAND_MAX);
    if (noise < errorRatio){
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