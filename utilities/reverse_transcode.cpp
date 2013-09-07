#include <vector>
using namespace std;

/* Reverse Transcode
 *
 * Reverse Transcode takes transcoded data and reverses the transcoding,
 * leaving the actual bits originally generated on the other side of the wire.
 *
 * The formula is simple, however there is a chance of noise which can take the form
 * of insertions, deletions and bit flips.  These must be dealt with.
 *
 * If there was no noise the process would simply be:
 *
 * for every two bits:
 *    if 10, output 1
 *    if 01, output 0
 */

class ReverseTranscode
{

};
