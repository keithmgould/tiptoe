#include <UnitTest++.h>
#include <iostream>
#include "../transcode.cpp"
#define T (true)
#define F (false)

SUITE (transcode) {
  TEST(perform)
  {
    //Create a bunch of random byte input
    vector<unsigned char> byteInput;
    byteInput.push_back ( 0x00 ); // 00000000
    byteInput.push_back ( 0xFF ); // 11111111
    byteInput.push_back ( 0x5D ); // 01011101
    vector<bool> transcodedBits;
    Transcode::Perform(byteInput, transcodedBits);
    bool correctBits[48] =   {F,T,F,T,F,T,F,T,F,T,F,T,F,T,F,T,
                              T,F,T,F,T,F,T,F,T,F,T,F,T,F,T,F,
                              F,T,T,F,F,T,T,F,T,F,T,F,F,T,T,F};
    vector<bool>::iterator it = transcodedBits.begin();
    for(int i=0; i < 48; i++)
    {
      CHECK_EQUAL(correctBits[i], *it);
      it+= 1;
    }
  }
}
