#include "../include/unpacker.h"

/* Unpack the raw data from the packet
 *
 * Note the preamble is already removed, used by the
 * extractor to determine the boundries of this packet.
 */

void Unpacker::unpack(vector<bool> &packet, vector<bool> &data)
{
  // first extract the parity bits by performing untranscode until
  // we have 7 bits of untranscoded data.  We pay attention to how
  // many of the packets bits were used to do this.
  int bitsUsed;
  vector<bool> parityBits;
  Untranscoder::untranscode(packet, parityBits, bitsUsed, 7);

  // extract the remaining 90 bit payload, starting after the
  // bits that were used to hold the parity bits, and tossing
  // any buffer afterwards
  vector<bool> payload;
  payload.assign(packet.begin() + bitsUsed, packet.begin() + bitsUsed + 90);

  // ensure the payload is 120 bits by adding 0s
  int remaining = 120 - payload.size();
  for(int i=0; i < remaining; i++)
  {
    payload.push_back(0);
  }
  // now do a parity check against the remaining 120 bit payload
  vector<bool> correctedPayload;
  hamming.decode(payload, correctedPayload);

  // finally untranscode the corrected payload
  Untranscoder::untranscode(correctedPayload, data, bitsUsed);
}

