#include <iostream>
#include <time.h>
#include "p.h"
#include "encoder.h"
#include "channel.h"
using namespace std;

int main(int arc, char * arv[]){
  srand(time(NULL));
  char *s;
  // char s1[100];
  message * m;
  // message m1;
  s = new char[strlen("message") + 1];
  strcpy(s, "message");

  P p1(s);
  P p2;
  Encoder enc1;
  Encoder enc2;
  Channel c(0.5);

  // P1 get message from user
  p1.writeString(s);
  
  // p1 send message to enc1
  enc1.receiveMessage(*p1.readString());
  m = enc1.getMessage();
  cout << "Encoder text: " << m->text << endl;
  cout << "Encoder check: " << m->checksum << endl;

  // enc1 send message to channel
  c.receiveMessage(*enc1.getMessage());
  cout << "Channel text: \n" << m->text << endl;
  cout << "Channel check: \n" << m->checksum << endl;

  // channel send message to enc2
  m = c.transmitMessage();
  cout << "\nDecoding: " << enc2.decodeMessage(*m) << endl;

  // enc2 send message to p2
  p2.writeString(enc2.deliveryMessage());

  // Print result
  cout << "Final message: " << p2.readString() << endl;

  delete s;
  return 0;
}