#ifndef DELTA_FINDER_H
#define DELTA_FINDER_H

#include "constants.h"
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

class DeltaFinder
{
  public:
  static void Perform(vector<float>& inputSamples, int sampleCount, int sampleRate, vector<float>& deltas, float *timeAfterLastBuffersLastCrossing);
};

#endif
