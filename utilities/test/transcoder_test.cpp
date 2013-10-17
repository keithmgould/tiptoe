#include "gmock/gmock.h"
#include "../include/constants.h"
#include "../src/transcoder.cpp"

TEST (transcoder, transcode)
{
  bool rawBitsArr[24] = { 0,0,0,0,0,0,0,0,
                          1,1,1,1,1,1,1,1,
                          0,1,0,1,1,1,0,1 };
  vector<bool> rawBits (rawBitsArr, rawBitsArr + sizeof(rawBitsArr) / sizeof(bool) );
  vector<bool> transcodedBits;
  Transcoder::Transcode(rawBits, transcodedBits);
  bool correctBitsArr[40] =   {0,0,1,0,1,0,1,0,1,0,1,0,1,0,1, // data: 0x00
                               1,1,0,1,0,1,0,1,0,1,0,1,0,1,0, // data: 0xFF
                               0,1,0,1,1,0,1,0,0,1};          // data: 0x5D


  vector<bool> correctBits (correctBitsArr, correctBitsArr + sizeof(correctBitsArr) / sizeof(bool) );
  ASSERT_THAT(correctBits, testing::ContainerEq(transcodedBits));
}
