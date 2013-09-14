#include <UnitTest++.h>
#include "../constants.cpp"
#include "convert_test.cpp"
// #include "delta_finder_test.cpp"
#include "demodulate_test.cpp"
#include "downsample_test.cpp"
#include "transcode_test.cpp"
// #include "transmit_test.cpp"
#include "extract_test.cpp"
#include "reverse_transcode_test.cpp"

int main(int, char const *[])
{
  return UnitTest::RunAllTests();
}
