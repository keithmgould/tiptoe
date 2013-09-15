#include <UnitTest++.h>
#include "../convert.cpp"

SUITE (convert) {
  TEST(unsigned_char_to_bits) {
    unsigned char inputChars[3] = { 0xD3, 0xA9, 0x5D };
    vector<bool> dataBits;
    Convert::UnsignedCharToBits(inputChars, dataBits, 3);
    bool correctBitsArr[24] = {T,T,F,T,F,F,T,T,T,F,T,F,T,F,F,T,F,T,F,T,T,T,F,T};
    vector<bool> correctBits (correctBitsArr, correctBitsArr + sizeof(correctBitsArr) / sizeof(bool) );
    CHECK(correctBits == dataBits);
  }

  TEST(bits_to_unsigned_char)
  {
    bool inputBitsArr[24] = {F,T,F,F,T,F,T,T, F,T,F,F,T,T,F,T,  F,T,F,F,F,T,T,T};
    vector<bool> inputBits (inputBitsArr, inputBitsArr + sizeof(inputBitsArr) / sizeof(bool) );
    unsigned char correctBytes[3] = { 0x4B, 0x4D, 0x47 };
    unsigned char outputBytes[3];
    Convert::BitsToUnsignedChar(inputBits, outputBytes, 3);
    CHECK_ARRAY_EQUAL(correctBytes, outputBytes,3);
  }
}
