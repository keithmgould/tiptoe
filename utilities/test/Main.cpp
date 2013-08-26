#include <UnitTest++.h>
#include "transcode_test.cpp"
#include "downsample_test.cpp"
#include "delta_finder_test.cpp"
#include "demodulate_test.cpp"

int main(int, char const *[])
{
  return UnitTest::RunAllTests();
}
