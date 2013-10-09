#include "gmock/gmock.h"
#include "../src/constants.cpp"
#include "../src/convert.cpp"

TEST(convert, unsigned_char_to_bits) {
  unsigned char inputChars[3] = { 0xD3, 0xA9, 0x5D };
  vector<bool> dataBits;
  Convert::UnsignedCharToBits(inputChars, dataBits, 3);
  bool correctBitsArr[24] = {1,1,0,1,0,0,1,1,1,0,1,0,1,0,0,1,0,1,0,1,1,1,0,1};
  vector<bool> correctBits (correctBitsArr, correctBitsArr + sizeof(correctBitsArr) / sizeof(bool) );
  ASSERT_EQ(correctBits, dataBits);
}

TEST(convert, bits_to_unsigned_char)
{
  bool inputBitsArr[24] = {0,1,0,0,1,0,1,1, 0,1,0,0,1,1,0,1,  0,1,0,0,0,1,1,1};
  vector<bool> inputBits (inputBitsArr, inputBitsArr + sizeof(inputBitsArr) / sizeof(bool) );
  unsigned char correctBytes[3] = { 0x4B, 0x4D, 0x47 };
  unsigned char outputBytes[3];
  Convert::BitsToUnsignedChar(inputBits, outputBytes, 3);
  // CHECK_ARRAY_EQUAL(correctBytes, outputBytes,3);
}
