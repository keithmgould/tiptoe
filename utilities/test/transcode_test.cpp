#include <UnitTest++.h>
#include <iostream>
#include "../transcode.cpp"
#define T (true)
#define F (false)

SUITE (transcode)
{
  TEST(Double)
  {
    //Create a bunch of random byte input
    // 00000000, 11111111, 01011101
    unsigned char byteInput[3] = { 0x00, 0xFF, 0x5D};
    bool transcodedBits[48];
    Transcode::Double(byteInput, transcodedBits, 3);
    bool correctBits[48] =   {F,T,F,T,F,T,F,T,F,T,F,T,F,T,F,T,
                              T,F,T,F,T,F,T,F,T,F,T,F,T,F,T,F,
                              F,T,T,F,F,T,T,F,T,F,T,F,F,T,T,F};
    for(int i=0; i < 48; i++)
    {
      CHECK_EQUAL(correctBits[i], transcodedBits[i]);
    }
  }

  TEST(extract_with_perfect_transmission)
  {

    bool transcodedBits[18] = {F,T,F,T,F,F,T,T,F,T,T,T,F,T,F,F,T,F};
    bool dataBits[10];
    Transcode::Extract(transcodedBits, dataBits, 18);
  }
}
