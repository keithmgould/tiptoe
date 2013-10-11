#include <iostream>
#include "../include/constants.h"
#include "../include/convert.h"

void Convert::UnsignedCharToBits(unsigned char inputBytes[], vector<bool> &dataBits, int byteCount)
{
  for(int i = 0; i < byteCount; i++)
  {
    bitset<8> bits = bitset<8>(inputBytes[i]);
    for(int j=7; j >=0; j--)
    {
      if(bits[j] == 1)
      {
        dataBits.push_back(1);
      }else{
        dataBits.push_back(0);
      }
    }
  }
}

void Convert::BitsToUnsignedChar(vector<bool> &inputBits, unsigned char outputBytes[], int byteCount)
{
  octet o;
  for(int i=0; i<byteCount; i++ )
  {
    o.a = inputBits[8*i+0];
    o.b = inputBits[8*i+1];
    o.c = inputBits[8*i+2];
    o.d = inputBits[8*i+3];
    o.e = inputBits[8*i+4];
    o.f = inputBits[8*i+5];
    o.g = inputBits[8*i+6];
    o.h = inputBits[8*i+7];
    outputBytes[i] = o.byteVal;
  }
}
