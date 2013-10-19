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
  Untranscoder::Untranscode(packet, parityBits, bitsUsed, 7);

  // add the raw parity bits to the payload, as hamming.decode
  // expects the form: parity bits | data bits
  vector<bool> payload;
  payload.assign( parityBits.begin(), parityBits.end() );

  // extract the remaining 90 bit payload, starting after the
  // bits that were used to hold the parity bits, and tossing
  // any buffer afterwards
  payload.insert(payload.end(), packet.begin() + bitsUsed, packet.begin() + bitsUsed + 90);

  // ensure the payload is 127 bits by adding 0s
  int remaining = 127 - payload.size();
  for(int i=0; i < remaining; i++)
  {
    payload.push_back(0);
  }

  // now do a parity check against the remaining 120 bit payload
  vector<bool> correctedPayload;
  hamming.decode(payload, correctedPayload);

  // finally untranscode the corrected payload
  Untranscoder::Untranscode(correctedPayload, data, bitsUsed, 48);
}

