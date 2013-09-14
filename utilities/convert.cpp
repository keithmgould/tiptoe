#include <bitset>
#include <vector>
#include <iostream>

using namespace std;
class Convert {
  public:

  static void UnsignedCharToBits(unsigned char inputBytes[], vector<bool> &dataBits, int byteCount)
  {
    for(int i = 0; i < byteCount; i++)
    {
      bitset<8> bits = bitset<8>(inputBytes[i]);
      for(int j=7; j >=0; j--)
      {
        if(bits[j] == 1)
        {
          dataBits.push_back(T);
        }else{
          dataBits.push_back(F);
        }
      }
    }
  }
};
