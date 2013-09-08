#include <UnitTest++.h>
#include "../transmit.cpp"

SUITE (transmitter)
{
  TEST(constructor)
  {
    Transmitter blaster(1280);
    CHECK_EQUAL(14217,blaster.tContainer.waveforms[VLOW].at(1));
    CHECK_EQUAL(15227,blaster.tContainer.waveforms[LOW].at(1));
    CHECK_EQUAL(16383,blaster.tContainer.waveforms[BASE].at(1));
    CHECK_EQUAL(17715,blaster.tContainer.waveforms[HIGH].at(1));
  }
}
