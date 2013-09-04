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

#define T (true)
#define F (false)
#include <vector>
using namespace std;

class Extract
{
  public:
  int preambleIndex;
  vector<bool> transmittedBits;
  void findPreamble();
  void reverseTranscode();
  void storePostPreambleBits(vector<bool> &postPreambleBits);
  void stitch();
  Extract (vector<bool> &transmittedBits); // constructor
};

/*
 * The Constructor
 *
 * Takes the raw transmitted bits.
 */
Extract::Extract (vector<bool> &transmittedBits)
{
  this->preambleIndex = -1;
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
void Extract::findPreamble()
{
  for(int i = 3; i < this->transmittedBits.size(); i++)
  {
    if( this->transmittedBits.at(i-3) == true &&
        this->transmittedBits.at(i-2) == true &&
        this->transmittedBits.at(i-1) == true &&
        this->transmittedBits.at(i)   == false)
    {
      this->preambleIndex = i-3;
    }
  }
}

/* storePostPreambleBits
 *
 * The bits in this buffer found after the preamble will be used
 * in combination with the pre-preamble bits from the next buffer
 */
void Extract::storePostPreambleBits(vector<bool> &postPreambleBits)
{

}

/* stitch()
 *
 * A buffer from the other side of the wire will most likely span across
 * two local buffers.  Therefor to reconstruct the 96 bits we must
 * stitch the combined results of post-preamble data from the previous
 * buffer with pre-preamble data from the current buffer.
 *
 * TODO: what happens if (when) the preamble gets split between two receiving
 * buffers? Oy.
 */
void Extract::stitch()
{
  // if we don't know where the preamble is, we have nothing
  if(this->preambleIndex == -1) { return; }

  // first, store the data after the preamble
  int dataBeginsAt = this->preambleIndex + PREAMBLE_LENGTH;
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
