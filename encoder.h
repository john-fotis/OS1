#ifndef __ENCODER__H
#define __ENCODER__H

typedef struct{
  char * text;
  char * checksum;
  bool succesfull;
} message;

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