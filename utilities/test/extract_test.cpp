#include "gmock/gmock.h"
#include "../include/constants.h"
#include "../src/extract.cpp"

TEST(extract, constructor_with_empty_previousRawBits)
{
  bool newBitsArr[10] = {1,1,1,0,0,0,1,1,1,0};
  vector<bool> newRawBits (newBitsArr, newBitsArr + sizeof(newBitsArr) / sizeof(bool) );
  vector<bool> totalRawBits (newBitsArr, newBitsArr + sizeof(newBitsArr) / sizeof(bool) );
  vector<bool> previousRawBits;

  Extract extract(newRawBits, previousRawBits);
  ASSERT_TRUE(totalRawBits == extract.totalRawBits);
}

TEST(extract,constructor_with_populated_previousRawBits)
{
  bool newBitsArr[10] = {1,1,1,0,0,0,1,1,1,0};
  vector<bool> newRawBits (newBitsArr, newBitsArr + sizeof(newBitsArr) / sizeof(bool) );

  bool prevBitsArr[6] = {0,0,0,0,1,0};
  vector<bool> prevRawBits (prevBitsArr, prevBitsArr + sizeof(prevBitsArr) / sizeof(bool) );

  bool totalBitsArr[16] = {0,0,0,0,1,0,1,1,1,0,0,0,1,1,1,0};
  vector<bool> totalRawBits (totalBitsArr, totalBitsArr + sizeof(totalBitsArr) / sizeof(bool) );

  Extract extract(newRawBits, prevRawBits);
  ASSERT_TRUE(totalRawBits == extract.totalRawBits);
}

TEST(extract,findPreambles_with_no_preambles)
{
  bool totalBitsArr[20] = {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};
  vector<bool> totalRawBits (totalBitsArr, totalBitsArr + sizeof(totalBitsArr) / sizeof(bool) );
  vector<bool> foo;
  Extract extract(foo, foo);
  extract.totalRawBits = totalRawBits;
  pair <int, int> preambleIndexes = extract.findPreambles();
  ASSERT_TRUE(-1 == preambleIndexes.first);
  ASSERT_TRUE(-1 == preambleIndexes.second);
}

TEST(extract,findPreambles_with_1_preamble)
{
  bool totalBitsArr[20] = {0,1,0,1,1,1,1,0,0,1,0,1,0,1,0,1,0,1,0,1};
  vector<bool> totalRawBits (totalBitsArr, totalBitsArr + sizeof(totalBitsArr) / sizeof(bool) );
  vector<bool> foo;
  Extract extract(foo, foo);
  extract.totalRawBits = totalRawBits;
  pair <int, int> preambleIndexes = extract.findPreambles();
  ASSERT_TRUE( 3 == preambleIndexes.first);
  ASSERT_TRUE(-1 == preambleIndexes.second);
}

TEST(extract,findPreambles_with_2_preambles)
{
  bool totalBitsArr[20] = {0,1,0,1,1,1,1,0,0,1,0,1,0,1,0,1,1,1,1,0};
  vector<bool> totalRawBits (totalBitsArr, totalBitsArr + sizeof(totalBitsArr) / sizeof(bool) );
  vector<bool> foo;
  Extract extract(foo, foo);
  extract.totalRawBits = totalRawBits;
  pair <int, int> preambleIndexes = extract.findPreambles();
  ASSERT_TRUE( 3 == preambleIndexes.first);
  ASSERT_TRUE(15 == preambleIndexes.second);
}

TEST(extract,perform_with_no_preamble_indexes)
{
  bool totalBitsArr[20] = {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};
  vector<bool> totalRawBits (totalBitsArr, totalBitsArr + sizeof(totalBitsArr) / sizeof(bool) );
  vector<bool> extractedRawBits;
  vector<bool> remainingRawBits;
  vector<bool> foo;
  Extract extract(foo, foo);
  extract.totalRawBits = totalRawBits;
  extract.perform(extractedRawBits, remainingRawBits);
  ASSERT_TRUE(extractedRawBits.size() == 0);
  ASSERT_TRUE(totalRawBits == remainingRawBits);
}

TEST(extract,perform_with_1_preamble_indexes)
{
  bool totalBitsArr[20] = {0,1,0,1,0,1,1,1,1,0,0,1,0,1,0,1,0,1,0,1};
  vector<bool> totalRawBits (totalBitsArr, totalBitsArr + sizeof(totalBitsArr) / sizeof(bool) );

  bool correctBitsArr[15] = {1,1,1,1,0,0,1,0,1,0,1,0,1,0,1};
  vector<bool> correctBits (correctBitsArr, correctBitsArr + sizeof(correctBitsArr) / sizeof(bool) );

  vector<bool> extractedRawBits;
  vector<bool> remainingRawBits;
  vector<bool> foo;
  Extract extract(foo, foo);
  extract.totalRawBits = totalRawBits;
  extract.perform(extractedRawBits, remainingRawBits);
  ASSERT_TRUE(extractedRawBits.size() == 0);
  ASSERT_TRUE(correctBits == remainingRawBits);
}

TEST(extract,perform_with_2_preamble_indexes)
{
  bool totalBitsArr[24] = {0,1,0,1,0,1,1,1,1,0,0,1,0,1,0,1,1,1,1,0,1,0,0,0};
  vector<bool> totalRawBits (totalBitsArr, totalBitsArr + sizeof(totalBitsArr) / sizeof(bool) );

  bool correctBitsArr[5] = {0,1,0,1,0};
  vector<bool> correctExtractedBits (correctBitsArr, correctBitsArr + sizeof(correctBitsArr) / sizeof(bool) );

  bool correctRemainingBitsArr[9] = {1,1,1,1,0,1,0,0,0};
  vector<bool> correctRemainingBits (correctRemainingBitsArr, correctRemainingBitsArr + sizeof(correctRemainingBitsArr) / sizeof(bool) );

  vector<bool> extractedRawBits;
  vector<bool> remainingRawBits;
  vector<bool> foo;
  Extract extract(foo, foo);
  extract.totalRawBits = totalRawBits;
  extract.perform(extractedRawBits, remainingRawBits);
  ASSERT_TRUE(correctExtractedBits == extractedRawBits);
  ASSERT_TRUE(correctRemainingBits == remainingRawBits);
}
