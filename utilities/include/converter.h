#ifndef CONVERTER_H
#define CONVERTER_H

#include <bitset>
#include <vector>
#include <iostream>

using namespace std;
class Converter
{
  public:
  static void UnsignedCharToBits(unsigned char inputBytes[], vector<bool> &dataBits, int byteCount);
  static void BitsToUnsignedChar(vector<bool> &inputBits, unsigned char outputBytes[], int byteCount);
  struct octet {
    union {
      char byteVal;
      struct {
        unsigned h : 1;
        unsigned g : 1;
        unsigned f : 1;
        unsigned e : 1;
        unsigned d : 1;
        unsigned c : 1;
        unsigned b : 1;
        unsigned a : 1;
      };
    };
  };
};

#endif

