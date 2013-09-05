#include <bitset>
#include <vector>
using namespace std;

#define T (true)
#define F (false)

/* Transcode
 *
 * Transcoding takes raw data and transcodes as follows:
 *
 * First it places the preamle.  The preamble is a series of
 * three zeros followed by three ones and a final false.  The
 * seven bits are required
 * because the transcoding below allows for two adjacent bits of
 * the same value.  That means that we need at least three adjacent
 * bits of the same value to establish a preamble.  However, the
 * preamble might get cut in half (worst case) on the other end
 * of the wire, so to ensure that it is still identifable, the
 * preamble must be six bits.  The final seventh bit leaves the
 * frequency at the base frequency.
 *
 * After the preamble is in place, the tanscoder takes the bytes
 * of data, and for each bit of each byte, transcodes the bit.
 *
 * The transcode rule is:
 * if 1, output 10
 * if 0, output 01
 *
 * Example:
 *   Input byte 0xD3.
 *   This would be 11010011 in binary.
 *   The transcoded bitstream with preamble
 *   would be: 1110001010011001011010.
 *
 */
class Transcode {
  public:

  static void Perform(unsigned char inputBytes[], vector<bool> &transcodedBits, int byteCount)
  {
    // First place the preamble
    transcodedBits.push_back(F);
    transcodedBits.push_back(F);
    transcodedBits.push_back(F);
    transcodedBits.push_back(T);
    transcodedBits.push_back(T);
    transcodedBits.push_back(T);
    transcodedBits.push_back(F);

    // Now us the transcode rule stated above.
    // This will double the length of the data.
    for(int i = 0; i < byteCount; i++)
    {
      bitset<8> bits = bitset<8>(inputBytes[i]);
      for(int j=7; j >=0; j--)
      {
        if(bits[j] == 1)
        {
          transcodedBits.push_back(T);
          transcodedBits.push_back(F);
        }else{
          transcodedBits.push_back(F);
          transcodedBits.push_back(T);
        }
      }
    }
  }
};
