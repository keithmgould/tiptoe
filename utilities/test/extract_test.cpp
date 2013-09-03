#include <UnitTest++.h>
#include "../extract.cpp"

SUITE (extract)
{
  TEST (constructor)
  {
    vector<bool> transmittedBits;
    transmittedBits.push_back(true);
    transmittedBits.push_back(false);
    Extract extract(transmittedBits);

    CHECK_EQUAL(extract.transmittedBits.at(0), true);
    CHECK_EQUAL(extract.transmittedBits.at(1), false);
  }

  TEST (findPreamble_preambleNotFound)
  {
    vector<bool> testInput;
    testInput.push_back(false);
    testInput.push_back(true);
    testInput.push_back(false);
    testInput.push_back(true);
    testInput.push_back(false);
    testInput.push_back(true);
    testInput.push_back(true);
    testInput.push_back(false);
    testInput.push_back(true);

    Extract extract(testInput);
    CHECK_EQUAL(-1, extract.findPreamble());
  }

  TEST (findPreamble_preambleFound)
  {
    vector<bool> testInput;
    testInput.push_back(false);
    testInput.push_back(true);
    testInput.push_back(false);
    testInput.push_back(true);
    testInput.push_back(true); // preamble begins here
    testInput.push_back(true);
    testInput.push_back(true);
    testInput.push_back(false);
    testInput.push_back(true);

    Extract extract(testInput);
    CHECK_EQUAL(4, extract.findPreamble());
  }
}
