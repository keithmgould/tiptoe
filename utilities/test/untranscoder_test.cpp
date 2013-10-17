#include "gmock/gmock.h"
#include "../include/constants.h"
#include "../src/untranscoder.cpp"

TEST (untranscoder, perform_with_less_than_max_bits) {
  bool extractedBitsArr[10] = { 1,0,0,0,1,0,1,0,1,1 };
  vector<bool> extractedBits (extractedBitsArr, extractedBitsArr + sizeof(extractedBitsArr) / sizeof(bool) );

  bool correctDataBitsArr[6] = { 1,0,0,0,0,1 };
  vector<bool> correctDataBits (correctDataBitsArr, correctDataBitsArr + sizeof(correctDataBitsArr) / sizeof(bool) );

  vector<bool> dataBits;
  Untranscoder::Untranscode(extractedBits, dataBits);
  ASSERT_TRUE(correctDataBits == dataBits);
}

TEST (untranscoder, perform_with_more_than_max_bits) {

  bool extractedBitsArr[10] = { 1,0,0,0,1,0,1,0,1,1 };
  vector<bool> extractedBits (extractedBitsArr, extractedBitsArr + sizeof(extractedBitsArr) / sizeof(bool) );

  bool correctDataBitsArr[5] = { 1,0,0,0,0 };
  vector<bool> correctDataBits (correctDataBitsArr, correctDataBitsArr + sizeof(correctDataBitsArr) / sizeof(bool) );

  vector<bool> dataBits;

  Untranscoder::Untranscode(extractedBits, dataBits, 5);
  ASSERT_TRUE(correctDataBits == dataBits);
}