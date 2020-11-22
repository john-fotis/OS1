#include <iostream>
#include "shared_memory.h"

using namespace std;

int main(int argc, char * argv[]) {
  // Checking input validity
  if (argc != 1) {
    cout << "Correct usage: $./destroy_memory.ex //No arguments" << endl;
    return -1;
  }

  if (destroyBlock(FILENAME)) {
    cout << "Destroyed block [" << FILENAME << "]" << endl;
  } else {
    cout << "Couldn't destroy block [" << FILENAME << "]" << endl;
    return -1;
  }

  return 0;
}