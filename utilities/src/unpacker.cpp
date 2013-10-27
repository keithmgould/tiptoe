#include "../include/unpacker.h"

/* Unpack the raw data from the packet
 *
 * Note the preamble is already removed, used by the
 * Extractor to determine the boundries of this packet.
 */

void Unpacker::unpack(vector<bool> &packet, vector<bool> &data)
{
  if(DEBUG_MODE > 1)
  {
    cout << "transmit raw bits (size 48): 110100111010100101011101001011011011110010010100" << endl;
    cout << "transmit transcoded bits (size 90): 101001001110100101001101011010010011011001100110101000110011010010101010101010101010101010" << endl;
    cout << "transmit raw parity bits (size 7): 1100010" << endl;
  }
  // first extract the parity bits by performing untranscode until
  // we have 7 bits of untranscoded data.  We pay attention to how
  // many of the packets bits were used to do this, which will range
  // from 7 to 14 bits.  We will store that information in the
  // bitsUsed int.
  int bitsUsed;
  vector<bool> parityBits;
  Untranscoder::Untranscode(packet, parityBits, bitsUsed, 7);
  if(DEBUG_MODE > 1)
  {
    Printer::print("packet", packet);
    Printer::print("parity", parityBits);
    cout << "raw bit count used for parity: " << bitsUsed << endl;
  }

  // add the raw parity bits to the payload, as hamming.decode
  // expects the form: parity bits | data bits
  vector<bool> payload;
  payload.assign( parityBits.begin(), parityBits.end() );
  if(DEBUG_MODE > 1){Printer::print("payload-0", payload); }

  // extract the remaining 90 bit payload, starting after the
  // bits that were used to hold the parity bits, and tossing
  // any buffer afterwards
  payload.insert(payload.end(), packet.begin() + bitsUsed, packet.end());
  if(DEBUG_MODE > 1){Printer::print("payload-1", payload); }

  // ensure the payload is 127 bits by adding 0s
  int remaining = 127 - payload.size();
  for(int i=0; i < remaining; i++)
  {
    payload.push_back(0);
  }

  if(DEBUG_MODE > 1){Printer::print("payload-2", payload); }
  // now do a parity check against the remaining 120 bit payload
  vector<bool> correctedPayload;
  hamming.decode(payload, correctedPayload);

  // finally untranscode the corrected payload, yielding the
  // 48 bits of actual data
  Untranscoder::Untranscode(correctedPayload, data, bitsUsed, 48);

  if(DEBUG_MODE > 1)
  {
    cout << "raw bits used for data: " << bitsUsed << endl;
    Printer::print("corrected payload", correctedPayload);
    Printer::print("data", data);
  }
}

