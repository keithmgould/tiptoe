#include "gmock/gmock.h"
#include "../include/constants.h"
#include "../src/untranscoder.cpp"

TEST (untranscoder, perform_with_less_than_max_bits) {
  bool extractedBitsArr[10] = { 1,0,0,0,1,0,1,0,1,1 };
  vector<bool> extractedBits (extractedBitsArr, extractedBitsArr + sizeof(extractedBitsArr) / sizeof(bool) );

  bool correctDataBitsArr[6] = { 1,0,0,0,0,1 };
  vector<bool> correctDataBits (correctDataBitsArr, correctDataBitsArr + sizeof(correctDataBitsArr) / sizeof(bool) );

  vector<bool> dataBits;
  int bitsUsed = 0;
  Untranscoder::Untranscode(extractedBits, dataBits, bitsUsed);
  ASSERT_TRUE(correctDataBits == dataBits);
  ASSERT_TRUE(bitsUsed == 10);
}

TEST (untranscoder, perform_with_more_than_max_bits_A) {
  bool extractedBitsArr[16] = { 1,0,1,0,0,0,1,0,1,1,0,1,0,1,0,1 };
  vector<bool> extractedBits (extractedBitsArr, extractedBitsArr + sizeof(extractedBitsArr) / sizeof(bool) );

  bool correctDataBitsArr[7] = { 1,1,0,0,0,1,0 };
  vector<bool> correctDataBits (correctDataBitsArr, correctDataBitsArr + sizeof(correctDataBitsArr) / sizeof(bool) );

  vector<bool> dataBits;
  int bitsUsed;
  Untranscoder::Untranscode(extractedBits, dataBits, bitsUsed, 7);
  ASSERT_TRUE(correctDataBits == dataBits);
  ASSERT_EQ(11, bitsUsed);
}

TEST (untranscoder, perform_with_more_than_max_bits_B) {

  bool extractedBitsArr[10] = { 1,0,0,0,1,0,1,0,1,1 };
  vector<bool> extractedBits (extractedBitsArr, extractedBitsArr + sizeof(extractedBitsArr) / sizeof(bool) );

  bool correctDataBitsArr[5] = { 1,0,0,0,0 };
  vector<bool> correctDataBits (correctDataBitsArr, correctDataBitsArr + sizeof(correctDataBitsArr) / sizeof(bool) );

  vector<bool> dataBits;
  int bitsUsed;
  Untranscoder::Untranscode(extractedBits, dataBits, bitsUsed, 5);
  ASSERT_TRUE(correctDataBits == dataBits);
  ASSERT_EQ(9, bitsUsed);
}
