#include <UnitTest++.h>
#include "../transcode.cpp"

#include <iostream>

SUITE (transcode)
{
  TEST(perform)
  {
    bool rawBitsArr[24] = { F,F,F,F,F,F,F,F,
                            T,T,T,T,T,T,T,T,
                            F,T,F,T,T,T,F,T };
    vector<bool> rawBits (rawBitsArr, rawBitsArr + sizeof(rawBitsArr) / sizeof(bool) );
    vector<bool> transcodedBits;
    Transcode::Perform(rawBits, transcodedBits);
    bool correctBitsArr[45] =   {T,T,T,T,F,                     // preamble
                                 F,F,T,F,T,F,T,F,T,F,T,F,T,F,T, // data: 0x00
                                 T,T,F,T,F,T,F,T,F,T,F,T,F,T,F, // data: 0xFF
                                 F,T,F,T,T,F,T,F,F,T};          // data: 0x5D


    vector<bool> correctBits (correctBitsArr, correctBitsArr + sizeof(correctBitsArr) / sizeof(bool) );
    CHECK(correctBits == transcodedBits);
  }
}
