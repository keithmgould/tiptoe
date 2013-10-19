#ifndef DEMODULATE_H
#define DEMODULATE_H

using namespace std;
#include <vector>

class Demodulate
{
  public:
  static void Perform(vector<float>& deltas, vector<bool>& outputBits);
};

#endif
