/* Extract
 *
 * Extract's job is the opposite of Transcode's job (and then some.)
 *
 * A Frame sent over the wire has three parts:
 *   1) The preamble (1110)
 *   2) The Data (48 bits transcoded to 96 bits)
 *   3) 1s and 0s alternating until the frame is complete
 *
 * Extract's job is to extract those 48 bits, in the face of noise.
 *
 */

#include <vector>
using namespace std;

class Extract
{
  public:
  vector<bool> transmittedBits;
  int findPreamble();
  void reverseTranscode();
  void stitch();
  Extract (vector<bool> &transmittedBits); // constructor
};

/*
 * The Constructor
 *
 * Takes the raw transmitted bits
 */
Extract::Extract (vector<bool> &transmittedBits)
{
  this->transmittedBits = transmittedBits;
}

/* findPreamble()
 *
 * Searches through the raw transmittedBits for
 * the preamble.
 *
 * Function returns the index of the start of the preamble.
 * If preamble not found it returns -1.
 *
 */
int Extract::findPreamble()
{
  for(int i = 3; i < this->transmittedBits.size(); i++)
  {
    if( this->transmittedBits.at(i-3) == true &&
        this->transmittedBits.at(i-2) == true &&
        this->transmittedBits.at(i-1) == true &&
        this->transmittedBits.at(i)   == false)
    {
      return i-3;
    }
  }
  return -1;
}

/* stitch()
 *
 * A frame from the other end of the wire will most likely span across
 * two buffers locally.  Therefor to reconstruct the 96 bits we must
 * stitch the results of the previous buffer with the results of this buffer.
 *
 * In particular, we must stitch the data found between the last two
 * preambles.
 */
void Extract::stitch()
{

}

/* reverseTranscode()
 *
 * once we have the data between two preambles, we must perform the reverse
 * action of Transcode on the raw bits.  Unfortunately there might be noise
 * and reverseTranscode must deal with insertions, deletions and bit flips.
 */

void Extract::reverseTranscode()
{

}
