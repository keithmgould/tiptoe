#include <UnitTest++.h>
#include "../downsample.cpp"

SUITE (downsample) {
  TEST(perform)
  {
    // create 16 samples of type short
    short input[16];
    for(int i=0; i< 16;i++)
    {
      input[i] = (short) (i * 2);
    }

    // create the output
    short output[4];

    // run the method
    Downsample::Perform(input, output, 16, 4);

    // ensure we have the correct 4 samples
    CHECK_EQUAL(0, output[0]);
    CHECK_EQUAL(8, output[1]);
    CHECK_EQUAL(16, output[2]);
    CHECK_EQUAL(24, output[3]);

  }
}
