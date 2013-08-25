#include <bitset>
using namespace std;

/* Transcoder
 *
 * Trancode's job is to take bytes of data, and for each bit of each byte,
 * transcode the bit.  The transcode rule is:
 * if 1, output 10
 * if 0, output 01
 *
 * Example:
 * Input byte 0xD3.
 * This would be 11010011 in binary.
 * The transcoded bitstream would be: 1010011001011010.
 *
 */
class Transcode {
  public:
  static void Perform(unsigned char inputBytes[], bool transcodedBits[], int byteCount)
  {
    int bitIndex = 0;
    for(int i = 0; i < byteCount; i++)
    {
      bitset<8> bits = bitset<8>(inputBytes[i]);
      for(int j=7; j >=0; j--)
      {
        if(bits[j] == 1)
        {
          transcodedBits[bitIndex++] = 1;
          transcodedBits[bitIndex++] = 0;
        }else{
          transcodedBits[bitIndex++] = 0;
          transcodedBits[bitIndex++] = 1;
        }
      }
    }
  }
};
