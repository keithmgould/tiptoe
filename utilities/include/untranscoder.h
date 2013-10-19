#ifndef UNTRANSCODER_H
#define UNTRANSCODER_H

#include <vector>
#include "constants.h"
using namespace std;

class Untranscoder
{
  public:
  static void Untranscode(vector<bool> &transcoded, vector<bool> &data, int &bitsUsed, int maxBits = -1);
};

#endif
