#ifndef __ENCODER__H
#define __ENCODER__H

#include "shared_memory.h"

class Encoder{
private:
  message * m;
  void setMessageFaulty(message &m);
public:
  Encoder();
  ~Encoder();
  void receiveMessage(const char &txt);
  bool decodeMessage(const message &m);
  message * getMessage();
  char * deliveryMessage();
};

#endif