#include <UnitTest++.h>
#include "../delta_finder.cpp"

SUITE (delta_finder)
{
  TEST (perform)
  {
    float inputSamples[10] = { -0.100, -0.010, 0.30, 0.50, 0.20, -0.030, -0.60, -0.020, 0.020, 0.20 };
    int sampleRate = 32000;
    vector<float> outputDeltas;
    float deltaAfterLastCrossing = 0.00202;
    DeltaFinder::Perform(inputSamples, 10, sampleRate, outputDeltas, &deltaAfterLastCrossing);
    CHECK_CLOSE(0.00205226,  outputDeltas.at(0), 0.00000001);
    CHECK_CLOSE(0.000202117, outputDeltas.at(1), 0.00000001);
  }
}
