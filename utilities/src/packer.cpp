#include "../include/packer.h"
/*
  The Packer will create the "packet" sent across the wire/air.
  The packet is 107 bits and structured as follows:

  bits - usage
  5    - preamble
  7-14 - 7 bit parity hamming error correctig code (transcoded), based on transcodedBits
  90   - transcodedBits.
*/

/*
  Arguments:

  rawBits: the incoming raw data.  48 bits long.
  packet: the outgoing transcoded bits.  < 107 bits long.
*/
void Packer::build(vector<bool> &rawBits, vector<bool> &packet)
{
  // first add the 5 preamble bits to the final packet
  add_preamble_to_packet(packet);

  // transcode the raw 48 bits of data
  vector<bool> transcodedBits;
  Transcoder::Transcode(rawBits, transcodedBits);

  // if transcoded data is < 90 bits, buffer
  add_buffer_bits(transcodedBits, 90);

  // determine the 7 parity bits for the 90 transcoded bits
  vector<bool> parityBits;
  determine_parity_bits(transcodedBits, parityBits);

  // transcode the 7 parity bits
  vector<bool> transcodedParityBits;
  Transcoder::Transcode(parityBits, transcodedParityBits);

  // add the transcoded parity bits
  add_bits_to_packet(transcodedParityBits, packet);

  // finally add the transcoded data
  add_bits_to_packet(transcodedBits, packet);

  // all done.  Debugging info here...
  if (DEBUG_MODE > 1 )
  {
    Printer::print("raw bits", rawBits);
    Printer::print("transcoded bits", transcodedBits);
    Printer::print("raw parity bits", parityBits);
    Printer::print("transcoded parity bits", transcodedParityBits);
    Printer::print("packet bits", packet);
    cout << endl;
  }
}

void Packer::add_bits_to_packet(vector<bool> &bits, vector<bool> &packet)
{
  packet.insert(packet.end(), bits.begin(), bits.end());
}

void Packer::add_preamble_to_packet(vector<bool> &packet)
{
  packet.push_back(1);
  packet.push_back(1);
  packet.push_back(1);
  packet.push_back(1);
  packet.push_back(0);
}

/*
 add alternating 1s and 0s until there are total_size bits.
 */
void Packer::add_buffer_bits(vector<bool> &transcodedBits, int total_size)
{
  int val = 0;
  while(transcodedBits.size() < total_size)
  {
    transcodedBits.push_back(val);
    val = (val + 1) % 2;
  }
}

void Packer::determine_parity_bits(vector<bool> &transcodedBits, vector<bool> &parityBits)
{
  int size = transcodedBits.size();
  // the hamming 7 parity bit encoder wants 120 bits
  int remaining = 120 - transcodedBits.size();
  for(int i=0; i < remaining; i++)
  {
    transcodedBits.push_back(0);
  }

  // cout << "tb: ";
  // for(int i=0; i< transcodedBits.size();i++)
  // {
    // cout << transcodedBits.at(i);
  // }
  // cout << endl;

  vector<bool> encodedBits;
  hamming.encode(transcodedBits, encodedBits);

  // now cut off those extra zeros from transcodedBits
  transcodedBits.resize(size);

  // cout << "eb: ";
  // for(int i=0; i< encodedBits.size();i++)
  // {
    // cout << encodedBits.at(i);
  // }
  // cout << endl;
  parityBits.assign(encodedBits.begin(), encodedBits.begin()+7);
}
