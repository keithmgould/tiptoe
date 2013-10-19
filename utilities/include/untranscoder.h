#ifndef UNTRANSCODER_H
#define UNTRANSCODER_H

#include <vector>

using namespace std;

class Untranscoder
{
  public:
  static void Untranscode(vector<bool> &transcoded, vector<bool> &data, int &bitsUsed, int maxBits = -1);
};

#endif
