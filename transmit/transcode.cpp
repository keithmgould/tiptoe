#include <vector>
#include <bitset>
using namespace std;

class Transcode {
  public:
  static void Perform(vector<unsigned char> &compressedBytes, vector<bool> &transcodedBits)
  {
    for(vector<unsigned char>::iterator i = compressedBytes.begin(); i != compressedBytes.end(); i++)
    {
      bitset<8> bits = bitset<8>(*i);
      for(int j=7; j >=0; j--)
      {
        if(bits[j] == 1)
        {
          transcodedBits.push_back(1);
          transcodedBits.push_back(0);
        }else{
          transcodedBits.push_back(0);
          transcodedBits.push_back(1);
        }
      }
    }
  }
};
