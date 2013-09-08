#include <UnitTest++.h>
#include <iostream>
#include "../extract.cpp"

SUITE (extract)
{
  TEST (constructor_with_empty_previousRawBits)
  {
    bool newBitsArr[10] = {T,T,T,F,F,F,T,T,T,F};
    vector<bool> newRawBits (newBitsArr, newBitsArr + sizeof(newBitsArr) / sizeof(bool) );
    vector<bool> totalRawBits (newBitsArr, newBitsArr + sizeof(newBitsArr) / sizeof(bool) );
    vector<bool> previousRawBits;

    Extract extract(newRawBits, previousRawBits);
    CHECK(totalRawBits == extract.totalRawBits);
  }

  TEST (constructor_with_populated_previousRawBits)
  {
    bool newBitsArr[10] = {T,T,T,F,F,F,T,T,T,F};
    vector<bool> newRawBits (newBitsArr, newBitsArr + sizeof(newBitsArr) / sizeof(bool) );

    bool prevBitsArr[6] = {F,F,F,F,T,F};
    vector<bool> prevRawBits (prevBitsArr, prevBitsArr + sizeof(prevBitsArr) / sizeof(bool) );

    bool totalBitsArr[16] = {F,F,F,F,T,F,T,T,T,F,F,F,T,T,T,F};
    vector<bool> totalRawBits (totalBitsArr, totalBitsArr + sizeof(totalBitsArr) / sizeof(bool) );

    Extract extract(newRawBits, prevRawBits);
    CHECK(totalRawBits == extract.totalRawBits);
  }

  TEST (findPreambles_with_no_preambles)
  {
    bool totalBitsArr[20] = {F,T,F,T,F,T,F,T,F,T,F,T,F,T,F,T,F,T,F,T};
    vector<bool> totalRawBits (totalBitsArr, totalBitsArr + sizeof(totalBitsArr) / sizeof(bool) );
    vector<bool> foo;
    Extract extract(foo, foo);
    extract.totalRawBits = totalRawBits;
    pair <int, int> preambleIndexes = extract.findPreambles();
    CHECK_EQUAL(-1, preambleIndexes.first);
    CHECK_EQUAL(-1, preambleIndexes.second);
  }

  TEST (findPreambles_with_1_preamble)
  {
    bool totalBitsArr[20] = {F,T,F,T,T,T,T,F,F,T,F,T,F,T,F,T,F,T,F,T};
    vector<bool> totalRawBits (totalBitsArr, totalBitsArr + sizeof(totalBitsArr) / sizeof(bool) );
    vector<bool> foo;
    Extract extract(foo, foo);
    extract.totalRawBits = totalRawBits;
    pair <int, int> preambleIndexes = extract.findPreambles();
    CHECK_EQUAL( 3, preambleIndexes.first);
    CHECK_EQUAL(-1, preambleIndexes.second);
  }

  TEST (findPreambles_with_2_preambles)
  {
    bool totalBitsArr[20] = {F,T,F,T,T,T,T,F,F,T,F,T,F,T,F,T,T,T,T,F};
    vector<bool> totalRawBits (totalBitsArr, totalBitsArr + sizeof(totalBitsArr) / sizeof(bool) );
    vector<bool> foo;
    Extract extract(foo, foo);
    extract.totalRawBits = totalRawBits;
    pair <int, int> preambleIndexes = extract.findPreambles();
    CHECK_EQUAL( 3, preambleIndexes.first);
    CHECK_EQUAL(15, preambleIndexes.second);
  }

  TEST (perform_with_no_preamble_indexes)
  {
    bool totalBitsArr[20] = {F,T,F,T,F,T,F,T,F,T,F,T,F,T,F,T,F,T,F,T};
    vector<bool> totalRawBits (totalBitsArr, totalBitsArr + sizeof(totalBitsArr) / sizeof(bool) );
    vector<bool> extractedRawBits;
    vector<bool> remainingRawBits;
    vector<bool> foo;
    Extract extract(foo, foo);
    extract.totalRawBits = totalRawBits;
    extract.perform(extractedRawBits, remainingRawBits);
    CHECK(extractedRawBits.size() == 0);
    CHECK(totalRawBits == remainingRawBits);
  }

  TEST (perform_with_1_preamble_indexes)
  {
    bool totalBitsArr[20] = {F,T,F,T,F,T,T,T,T,F,F,T,F,T,F,T,F,T,F,T};
    vector<bool> totalRawBits (totalBitsArr, totalBitsArr + sizeof(totalBitsArr) / sizeof(bool) );

    bool correctBitsArr[15] = {T,T,T,T,F,F,T,F,T,F,T,F,T,F,T};
    vector<bool> correctBits (correctBitsArr, correctBitsArr + sizeof(correctBitsArr) / sizeof(bool) );

    vector<bool> extractedRawBits;
    vector<bool> remainingRawBits;
    vector<bool> foo;
    Extract extract(foo, foo);
    extract.totalRawBits = totalRawBits;
    extract.perform(extractedRawBits, remainingRawBits);
    CHECK(extractedRawBits.size() == 0);
    CHECK(correctBits == remainingRawBits);
  }

  TEST (perform_with_2_preamble_indexes)
  {
    bool totalBitsArr[24] = {F,T,F,T,F,T,T,T,T,F,F,T,F,T,F,T,T,T,T,F,T,F,F,F};
    vector<bool> totalRawBits (totalBitsArr, totalBitsArr + sizeof(totalBitsArr) / sizeof(bool) );

    bool correctBitsArr[5] = {F,T,F,T,F};
    vector<bool> correctExtractedBits (correctBitsArr, correctBitsArr + sizeof(correctBitsArr) / sizeof(bool) );

    bool correctRemainingBitsArr[9] = {T,T,T,T,F,T,F,F,F};
    vector<bool> correctRemainingBits (correctRemainingBitsArr, correctRemainingBitsArr + sizeof(correctRemainingBitsArr) / sizeof(bool) );

    vector<bool> extractedRawBits;
    vector<bool> remainingRawBits;
    vector<bool> foo;
    Extract extract(foo, foo);
    extract.totalRawBits = totalRawBits;
    extract.perform(extractedRawBits, remainingRawBits);
    CHECK(correctExtractedBits == extractedRawBits);
    CHECK(correctRemainingBits == remainingRawBits);
  }
}
