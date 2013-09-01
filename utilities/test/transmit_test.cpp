#include <UnitTest++.h>
#include <iostream>
#include "../transmit.cpp"
#define T (true)
#define F (false)

SUITE (transmitter)
{
  TEST(constructor)
  {
    Transmitter blaster(1280);
    CHECK_EQUAL(14217,blaster.waveforms.vlow_sine[1]);
    CHECK_EQUAL(15227,blaster.waveforms.low_sine[1]);
    CHECK_EQUAL(16383,blaster.waveforms.base_sine[1]);
    CHECK_EQUAL(17715,blaster.waveforms.high_sine[1]);
  }

  TEST(determineNextNode)
  {

  }
}
