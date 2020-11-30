#ifndef __ENCODER__H
#define __ENCODER__H

#include "Shared_memory.h"

class Encoder{
private:
  message * m;
public:
  Encoder();
  ~Encoder();
  void encodeMessage(const message &msg);
  bool decodeMessage(const message &m);
  message * getMessage();
  char * deliveryMessage();
};

#endif