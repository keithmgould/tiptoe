#include <UnitTest++.h>
#include "../transcode.cpp"

#include <iostream>

SUITE (transcode)
{
  TEST(perform)
  {
    // random byte input
    // 0x00: 00000000
    // 0xFF: 11111111
    // 0x5D: 01011101
    unsigned char byteInput[3] = { 0x00, 0xFF, 0x5D };
    vector<bool> transcodedBits;
    Transcode::Perform(byteInput, transcodedBits, 3);
    bool correctBitsArr[54] =   {T,T,T,F,F,F,                       // preamble
                                 F,T,F,T,F,T,F,T,F,T,F,T,F,T,F,T,   // 0x00
                                 T,F,T,F,T,F,T,F,T,F,T,F,T,F,T,F,   // 0xFF
                                 F,T,T,F,F,T,T,F,T,F,T,F,F,T,T,F};  // 0x5D
    vector<bool> correctBits (correctBitsArr, correctBitsArr + sizeof(correctBitsArr) / sizeof(bool) );
    CHECK(correctBits == transcodedBits);
  }
}
