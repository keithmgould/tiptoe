#include <UnitTest++.h>
#include "delta_finder_test.cpp"
#include "demodulate_test.cpp"
#include "downsample_test.cpp"
#include "transcode_test.cpp"
#include "transmit_test.cpp"

int main(int, char const *[])
{
  return UnitTest::RunAllTests();
}
