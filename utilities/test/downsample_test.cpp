#include <UnitTest++.h>
#include <iostream>
#include "../downsample.cpp"
#define T (true)
#define F (false)

SUITE (downsample) {
  TEST(perform)
  {
    // create 16 samples of type short
    short input[16];
    for(int i=0; i< 16;i++)
    {
      input[i] = (short) i;
    }

    // create the output
    short output[4];

    // run the method
    Downsample::Perform(input, output, 16, 4);

    // ensure we have the correct 4 samples
    CHECK_EQUAL(0, output[0]);
    CHECK_EQUAL(4, output[1]);
    CHECK_EQUAL(8, output[2]);
    CHECK_EQUAL(12, output[3]);

  }
}
