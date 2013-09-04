#include <UnitTest++.h>
#include "../transcode.cpp"

SUITE (transcode)
{
  TEST(Double)
  {
    // random byte input
    // 00000000, 11111111, 01011101
    unsigned char byteInput[3] = { 0x00, 0xFF, 0x5D};
    vector<bool> transcodedBits;
    Transcode::Double(byteInput, transcodedBits, 3);
    // bool correctBits[48] =   {F,T,F,T,F,T,F,T,F,T,F,T,F,T,F,T,
                              // T,F,T,F,T,F,T,F,T,F,T,F,T,F,T,F,
                              // F,T,T,F,F,T,T,F,T,F,T,F,F,T,T,F};
    // for(int i=0; i < 48; i++)
    // {
      // CHECK_EQUAL(correctBits[i], transcodedBits[i]);
    // }
  }
}
