#ifndef __P__H
#define __P__H
#include <iostream>
#include <string.h>

class P{
private:
  char * txt;
public:
  P() {
    txt = new char [1];
    strcpy (txt, "");
  }
  void setMessage(char *s){
    delete [] txt;
    txt = new char [strlen(s)+1];
    strcpy (txt, s);
  }
  char * getMessage(){return txt;}
  ~P() {
    delete [] txt;
  }
};

#endif