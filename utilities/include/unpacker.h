#ifndef UNPACKER_H
#define UNPACKER_H

#include <vector>
#include "untranscoder.h"
using namespace std;

class Unpacker
{
  public:
  Unpacker() : hamming(7) {}
  void unpack(vector<bool> &packet, vector<bool> &dataBits);
  private:
  Hamming hamming;
}

#endif
