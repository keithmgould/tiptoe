#ifndef UNPACKER_H
#define UNPACKER_H

#include <vector>

using namespace std;

class Unpacker
{
  public:
  Unpacker() : hamming(7) {}
  void unpack(vector<bool> &packet, vector<bool> &dataBits, int maxBits);
  private:
  Hamming hamming;
}

#endif
