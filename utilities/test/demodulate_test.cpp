#include <UnitTest++.h>
#include "../demodulate.cpp"

SUITE (demodulate)
{
  TEST(perform)
  {
    vector<float> deltas;
    vector<bool> bits;
    deltas.push_back(0.001);
    deltas.push_back(0.0005);
    deltas.push_back(0.003);
    deltas.push_back(0.0004);
    deltas.push_back(0.002);
    Demodulate::Perform(deltas, bits);
    vector<bool>::iterator bitIterator = bits.begin();
    CHECK_EQUAL(*bitIterator++, false);
    CHECK_EQUAL(*bitIterator++, true);
    CHECK_EQUAL(*bitIterator++, false);
    CHECK_EQUAL(*bitIterator++, true);
  }
}
