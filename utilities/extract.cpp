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
#include <queue>
using namespace std;

class Extract
{
  public:
  void findPreamble();
  void reverseTranscode();
  void stitch();
  Extract (vector<bool> &transmittedBits); // constructor
};

/*
 * The Constructor
 *
 * Takes the raw transmitted bits
 */
Extract::Extract (vector<bool> *transmittedBits)
{
  queue<bool, deque<bool>> transmittedBitsQueue(deque<bool>(transmittedBits.begin(), transmittedBits.end());
}

/* findPreamble()
 *
 * Searches through the raw transmittedBits for
 * the preamble.
 *
 * To do this efficiently, we use the queue.
 */
Extract::findPreamble()
{

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
Extract::stitch()
{

}

/* reverseTranscode()
 *
 * once we have the data between two preambles, we must perform the reverse
 * action of Transcode on the raw bits.  Unfortunately there might be noise
 * and reverseTranscode must deal with insertions, deletions and bit flips.
 */

Extract::reverseTranscode()
{

}
