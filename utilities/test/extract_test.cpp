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
    extract.findPreamble();
    CHECK_EQUAL(-1, extract.preambleIndex);
  }

  TEST (findPreamble_preambleFound)
  {
    vector<bool> testInput;
    testInput.push_back(false);
    testInput.push_back(true);
    testInput.push_back(false); // preamble begins here
    testInput.push_back(false);
    testInput.push_back(false);
    testInput.push_back(true);
    testInput.push_back(true);
    testInput.push_back(true);
    testInput.push_back(false);
    testInput.push_back(true);

    Extract extract(testInput);
    extract.findPreamble();
    CHECK_EQUAL(2, extract.preambleIndex);
  }

  TEST (storePostPreambleBits)
  {
    CHECK_EQUAL(1,1);
  }

  TEST (stitch_with_no_data_from_previous_buffer)
  {
    CHECK_EQUAL(1,1);
  }

  TEST (stitch_with_data_from_previous_buffer)
  {
    CHECK_EQUAL(1,1);
  }

  TEST (stitch_where_preamble_split_between_buffers)
  {
    CHECK_EQUAL(1,1);
  }
}
