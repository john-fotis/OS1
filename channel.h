#ifndef __CHANNEL__H
#define __CHANNEL__H
#include "encoder.h"

class Channel{
private:
  float errorRatio;
  message * m;
  
public:
  Channel(float ratio = 0.0);
  ~Channel();
  void receiveMessage(const message &msg);
  message * pushBackMessage(); // TO DO: return faulty message to enc1
  message * transmitMessage(); // TO DO: send message to enc2
};

#endif