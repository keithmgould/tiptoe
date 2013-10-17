#include "gmock/gmock.h"
#include "../include/constants.h"
#include "../src/delta_finder.cpp"
#include <vector>

TEST (perform, basic_functionality)
{
  float inputSampleArray[10] = { -0.100, -0.010, 0.30, 0.50, 0.20, -0.030, -0.60, -0.020, 0.020, 0.20 };
  vector<float> inputSamples (inputSampleArray, inputSampleArray + sizeof(inputSampleArray) / sizeof(float) );
  int sampleRate = 32000;
  vector<float> outputDeltas;
  float deltaAfterLastCrossing = 0.00202;
  DeltaFinder::Perform(inputSamples, 10, sampleRate, outputDeltas, &deltaAfterLastCrossing);
  ASSERT_THAT(0.00205226,  testing::FloatNear(outputDeltas.at(0), 0.00000001));
  ASSERT_THAT(0.000202117, testing::FloatNear(outputDeltas.at(1), 0.00000001));
}
