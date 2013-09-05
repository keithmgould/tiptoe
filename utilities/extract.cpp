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
  vector<bool> stitchedBits;
  void findPreamble();
  void reverseTranscode();
  void storePostPreambleBits(vector<bool> &postPreambleBits);
  void stitch(vector<bool> &prePreambleBits);
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
  for(int i = 6; i < this->transmittedBits.size(); i++)
  {
    if( this->transmittedBits.at(i-6) == F &&
        this->transmittedBits.at(i-5) == F &&
        this->transmittedBits.at(i-4) == F &&
        this->transmittedBits.at(i-3) == T &&
        this->transmittedBits.at(i-2) == T &&
        this->transmittedBits.at(i-1) == T &&
        this->transmittedBits.at(i)   == F)
    {
      this->preambleIndex = i-6;
    }
  }
}

/* storePostPreambleBits
 *
 * The post-preamble bits in this buffer will be used
 * in combination with the pre-preamble bits in the next buffer
 */
void Extract::storePostPreambleBits(vector<bool> &postPreambleBits)
{
  // if we don't know where the preamble is, we have nothing
  if(this->preambleIndex == -1) { return; }

  // get to the end of the preamble
  vector<bool>::iterator endOfPreamble = this->transmittedBits.begin() + this->preambleIndex + 5;

  // from end of preamble to end of transmittedBits
  postPreambleBits.assign(endOfPreamble, this->transmittedBits.end());
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
void Extract::stitch(vector<bool> &prePreambleBits)
{
  // if we don't know where the preamble is, we have nothing
  if(this->preambleIndex == -1) { return; }

  // get to the end of the preamble
  vector<bool>::iterator beginningOfPreamble = this->transmittedBits.begin() + this->preambleIndex;

  // first add in the prePremable bits (the bits from after the last bufffers preamble)
  this->stitchedBits.assign(prePreambleBits.begin(), prePreambleBits.end());

  // then add in this buffers bits from before the preamble
  this->stitchedBits.insert(stitchedBits.end(), this->transmittedBits.begin(), beginningOfPreamble);
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
