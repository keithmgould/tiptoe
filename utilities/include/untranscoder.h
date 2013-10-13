#ifndef UNTRANSCODER_H
#define UNTRANSCODER_H

#include <vector>
using namespace std;

class Untranscoder
{
  public:
  Untranscoder() : bitsUsed(0) {}
  int bitsUsed;
  void untranscode(vector<bool> &transcoded, vector<bool> &data, int maxBits = -1)
}

#endif
