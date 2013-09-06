/* Extract
 *
 * Extract's job is the opposite of Transcode's job (and then some.)
 *
 * A Frame sent over the wire has three parts:
 *   1) The preamble: 0001110
 *   2) The Data: 96 bits (post-transcode)
 *   3) 1s and 0s alternating until the frame is complete
 *
 * Extract's job is to extract those 48 (pre-transcode) bits, in the face of noise.
 *
 */

#define T (true)
#define F (false)
#include <vector>
using namespace std;

class Extract
{
  public:
  int preambleBegin;
  int preambleEnd;
  vector<bool> transmittedBits;
  vector<bool> stitchedBits;
  void perform();
  void findPreamble();
  void reverseTranscode();
  void storePostPreambleBits(vector<bool> &postPreambleBits);
  void stitch(vector<bool> &prePreambleBits);
  Extract (vector<bool> &transmittedBits); // constructor
};

/*
 * The Constructor
 *
 * Takes the raw transmitted bits; the entire input buffer.
 */
Extract::Extract (vector<bool> &transmittedBits)
{
  this->preambleBegin = -1;
  this->preambleEnd   = -1;
  this->transmittedBits = transmittedBits;
}

/* perform()
 *
 * Perform runs through the steps necessary to extract the transcoded bits.
 *
 * Pseudo Code:
 *
 *  find Preamble
 *  if entire preamble found,
 *     rawData = stitch (data from previous buffer, data from current buffer)
 *  else
 *     rawData = extract based on preamble location
 *
 *  reverseTranscode(rawData)
 *
 */
void Extract::perform()
{
  findPreamble();
  if(this->preambleBegin > 0 && this->preambleEnd > 0)
  {
    stitch();
  }else{

  }
}

/* findPreamble()
 *
 * Searches through the raw transmittedBits for
 * the preamble.
 *
 * We have to consider three cases:
 * 1) The entire preamble is found.
 *    This can happen anywhere in the transmittedBits vector.
 *    In this case we must stitch the data from the previous buffer and the
 *    current buffer.
 *
 * 2) Only the "front" (consisting of "FFF") is found.
 *    This can only happen at the very end of the transmittedBits vector due to
 *    getting chopped off at the end.  This is a split preamble.  In this case
 *    we look at where the preamble was chopped off, and search for the chopped off
 *    segment at the beginning of the transmittedBits.  We extract starting from
 *    the point right after the segment.
 *
 * 3) Only the "back" (consisting of TTTF ) is found.
 *    This can only happen at the very beginning of the transmittedBits
 *    vector due to getting chopped off at the very beginning. This is a
 *    split preamble.  In this case, we know the entire 96 bits of pre-transcoded
 *    data is in the current buffer.  We extract starting from the point right
 *    after the end of the preamble.
 *
 */
void Extract::findPreamble()
{
  for(int i = 2; i < this->transmittedBits.size(); i++)
  {
    // search for the beginning of the preamble
    if( this->transmittedBits.at(i-2) == F &&
        this->transmittedBits.at(i-1) == F &&
        this->transmittedBits.at(i)   == F)
    {
      this->preambleBegin = i-2;
    }

    // search for the end of the preamble
    if( this->transmittedBits.at(i-2) == T &&
        this->transmittedBits.at(i-1) == T &&
        this->transmittedBits.at(i)   == T)
    {
      if( (this->transmittedBits.size() - i + 1 > 0) && this->transmittedBits.at(i+1) == F)
      {
        this->preambleEnd = i+1;
      }
    }
  }
}

/* storePostPreambleBits
 *
 * The post-preamble bits in this buffer will be used
 * in combination with the pre-preamble bits in the next buffer.
 *
 * This is only relevant when we do not have a split preamble.
 */
void Extract::storePostPreambleBits(vector<bool> &postPreambleBits)
{
  // if we don't know where the preamble is, we have nothing
  if(this->preambleBegin == -1) { return; }

  // get to the end of the preamble
  vector<bool>::iterator endOfPreamble = this->transmittedBits.begin() + this->preambleBegin + 5;

  // from end of preamble to end of transmittedBits
  postPreambleBits.assign(endOfPreamble, this->transmittedBits.end());
}

/* stitch()
 *
 * A buffer from the other side of the wire will most likely span across
 * two local buffers.  Therefor to reconstruct the 96 bits we must
 * stitch the combined results of post-preamble data from the previous
 * buffer with pre-preamble data from the current buffer.
 */
void Extract::stitch(vector<bool> &prePreambleBits)
{
  // if we don't know where the preamble is, we have nothing
  if(this->preambleBegin == -1) { return; }

  // get to the end of the preamble
  vector<bool>::iterator beginningOfPreamble = this->transmittedBits.begin() + this->preambleBegin;

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
