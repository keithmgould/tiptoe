#ifndef DEMODULATOR_H
#define DEMODULATOR_H

using namespace std;
#include <vector>

class Demodulator
{
  public:
  static void Perform(vector<float>& deltas, vector<bool>& outputBits);
};

#endif
