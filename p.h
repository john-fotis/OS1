#ifndef __P__H
#define __P__H
#include <iostream>
#include <string.h>

class P{
private:
  char * str;
public:
  P() {
    str = new char [1];
    strcpy (str, "");
    // std::cout << "Created P with name " << str << std::endl;
  }
  P(const char *s) {
    str = new char[strlen(s)+1];
    strcpy(str, s);
    // std::cout << "Created P with name " << str << std::endl;
  }
  void writeString(char *s){
    delete [] str;
    str = new char [strlen(s)+1];
    strcpy (str, s);
  }
  char * readString(){return str;}
  ~P() {
    // std::cout << "Deleting P with name " << str << std::endl;
    delete [] str;
  }
};

#endif