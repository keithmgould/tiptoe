#include <UnitTest++.h>
#include "../reverse_transcode.cpp"

SUITE(reverse_transcode)
{
  TEST (perform_with_less_than_48_data_bits) {
    bool extractedBitsArr[10] = { T,F,F,F,T,F,T,F,T,T };
    vector<bool> extractedBits (extractedBitsArr, extractedBitsArr + sizeof(extractedBitsArr) / sizeof(bool) );

    bool correctDataBitsArr[6] = { T,F,F,F,F,T };
    vector<bool> correctDataBits (correctDataBitsArr, correctDataBitsArr + sizeof(correctDataBitsArr) / sizeof(bool) );

    vector<bool> dataBits;

    ReverseTranscode rt(extractedBits);
    rt.perform(dataBits);
    CHECK(correctDataBits == dataBits);
  }

  TEST (perform_with_more_than_48_data_bits) {

  }
}
