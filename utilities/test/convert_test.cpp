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
}
