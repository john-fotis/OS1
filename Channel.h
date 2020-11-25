#ifndef __CHANNEL__H
#define __CHANNEL__H

#include "Shared_memory.h"

class Channel{
private:
  float errorRatio;
  message * m;
public:
  Channel(float ratio = 0.0);
  ~Channel();
  void receiveMessage(const message &msg);
  message * transferMessage();
};

#endif