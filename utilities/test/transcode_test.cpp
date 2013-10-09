#include "gmock/gmock.h"
#include "../constants.cpp"
#include "../transcode.cpp"

TEST (transcode, perform)
{
  bool rawBitsArr[24] = { 0,0,0,0,0,0,0,0,
                          1,1,1,1,1,1,1,1,
                          0,1,0,1,1,1,0,1 };
  vector<bool> rawBits (rawBitsArr, rawBitsArr + sizeof(rawBitsArr) / sizeof(bool) );
  vector<bool> transcodedBits;
  Transcode::Perform(rawBits, transcodedBits);
  bool correctBitsArr[45] =   {1,1,1,1,0,                     // preamble
                               0,0,1,0,1,0,1,0,1,0,1,0,1,0,1, // data: 0x00
                               1,1,0,1,0,1,0,1,0,1,0,1,0,1,0, // data: 0xFF
                               0,1,0,1,1,0,1,0,0,1};          // data: 0x5D


  vector<bool> correctBits (correctBitsArr, correctBitsArr + sizeof(correctBitsArr) / sizeof(bool) );
  ASSERT_TRUE(correctBits == transcodedBits);
}
