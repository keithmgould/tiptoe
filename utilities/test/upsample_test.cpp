#include <UnitTest++.h>
#include "../upsample.cpp"

SUITE (upsample)
{
  TEST (perform)
  {
    short inputSamples[4] = {100, 200, 300, 400};
    short outputSamples[13];
    short correctResults[13] = {100, 125, 150, 175, 200, 225, 250, 275, 300, 325, 350, 375, 400};
    Upsample::Perform(inputSamples, outputSamples, 4);
    CHECK_ARRAY_EQUAL(correctResults,outputSamples, 13);
  }
}
