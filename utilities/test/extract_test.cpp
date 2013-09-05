#include <UnitTest++.h>
#include "../extract.cpp"
#define T (true)
#define F (false)

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
    vector<bool> testInput;
    testInput.push_back(false);
    testInput.push_back(true);
    testInput.push_back(false); // preamble begins here
    testInput.push_back(false);
    testInput.push_back(false);
    testInput.push_back(true);
    testInput.push_back(true);
    testInput.push_back(true);
    testInput.push_back(false); // preamble ends here
    testInput.push_back(true);
    testInput.push_back(true);
    testInput.push_back(true);
    testInput.push_back(false);
    testInput.push_back(false);
    Extract extract(testInput);
    extract.preambleIndex = 4;
    vector<bool> correctPostBits;
    correctPostBits.push_back(true);
    correctPostBits.push_back(true);
    correctPostBits.push_back(true);
    correctPostBits.push_back(false);
    correctPostBits.push_back(false);
    vector<bool> postBits;
    extract.storePostPreambleBits(postBits);
    CHECK(correctPostBits == postBits);
  }

  TEST (stitch_with_data_from_previous_buffer)
  {
    vector<bool> prePreambleBits;
    prePreambleBits.push_back(false);
    prePreambleBits.push_back(true);
    prePreambleBits.push_back(true);

    vector<bool> testInput;
    testInput.push_back(false);
    testInput.push_back(false);
    testInput.push_back(false);
    testInput.push_back(true);
    testInput.push_back(false); // preamble begins here
    testInput.push_back(false);
    testInput.push_back(false);
    testInput.push_back(true);
    testInput.push_back(true);
    testInput.push_back(true);
    testInput.push_back(false); // preamble ends here
    testInput.push_back(true);
    Extract extract(testInput);
    extract.preambleIndex = 4;
    extract.stitch(prePreambleBits);

    vector<bool> correctBits;
    // these are the postPreamable bits from last buffer
    correctBits.push_back(false);
    correctBits.push_back(true);
    correctBits.push_back(true);
    // these are the pre-preamble bits from this buffer
    correctBits.push_back(false);
    correctBits.push_back(false);
    correctBits.push_back(false);
    correctBits.push_back(true);

    CHECK(correctBits == extract.stitchedBits);
  }

  TEST (stitch_where_preamble_split_between_buffers)
  {
    CHECK_EQUAL(1,1);
  }

  TEST (reverseTranscode)
  {
    CHECK_EQUAL(1,1);
  }
}
