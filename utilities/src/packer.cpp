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
  transcode_bits(rawBits, transcodedBits);

  // if transcoded data is < 90 bits, buffer
  add_buffer_bits(transcodedBits, 90);

  // determine the 7 parity bits for the 90 transcoded bits
  vector<bool> parityBits;
  determine_parity_bits(transcodedBits, parityBits);

  // cout << "pb of length " << parityBits.size() << ": " ;
  // for(int i=0; i< parityBits.size();i++)
  // {
    // cout << parityBits.at(i);
  // }
  // cout << endl;

  // transcode the 7 parity bits
  vector<bool> transcodedParityBits;
  transcode_bits(parityBits, transcodedParityBits);

  // cout << "transcoded of length: " << transcodedParityBits.size() << ": " ;
  // for(int i=0; i< transcodedParityBits.size();i++)
  // {
    // cout << transcodedParityBits.at(i);
  // }
  // cout << endl;


  // add the transcoded parity bits, then the transcoded data
  add_bits_to_packet(transcodedParityBits, packet);
  add_bits_to_packet(transcodedBits, packet);

  if (DEBUG_MODE > 1 )
  {
    cout << endl << "trn:";
    for(int i = 0; i< transcodedBits.size(); i++)
    {
      cout << transcodedBits.at(i);
    }
    cout << endl << endl;
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
  // cout << "eb: ";
  // for(int i=0; i< encodedBits.size();i++)
  // {
    // cout << encodedBits.at(i);
  // }
  // cout << endl;
  parityBits.assign(encodedBits.begin(), encodedBits.begin()+7);
}
/*
  Transcode Bits:

  The preamble leaves us at the Middle High frequency.
  The frequency variable is used to keep track of which frequency
  we are at in case we reach an "edge" frequency and need to
  "recover".  On the receiving side, frequency will be kept track
  of so when an edge bit is reached, it will know to throw
  out this "recovery bit"

  The rawBits count will always be 6 bytes or 48 bits.
  The transcoding algorithm sends out an an average of
  1.5 times the input amount or 72 bits.  The protocol
  allocates 90 bits.  Remaining bits are filled with
  alternating ones and zeros until 90 bits are reached.

  Long strings of consecutive ones or zeros in the original
  data will lead to more "edge" frequencies, and therefor more wasted
  "recovery" bits, and therefore longer transcoded sequences.
  Thusly there is a very very small chance that the transcoded
  sequence is above 90 bits.

  TODO: I'm not sure what to do about this yet...

*/
void Packer::transcode_bits(vector<bool> &rawBits, vector<bool> &transcodedBits)
{
  int frequency = MIDDLE_HIGH;

  for(int i = 0; i < rawBits.size(); i++)
  {
    // Here we have a direct mapping between input and output
    if(DEBUG_MODE > 1) { cout << rawBits.at(i); }
    if(rawBits.at(i) == 1)
    {
      transcodedBits.push_back(1);
      frequency++;
    }else{
      transcodedBits.push_back(0);
      frequency--;
    }

    // If we ended up on an "edge" frequency,
    // recover by going in the other direction.
    // This recovery bit will be thrown out on
    // the other side.
    if(frequency == EDGE_LOW)
    {
      transcodedBits.push_back(1);
      frequency++;
    }
    if(frequency == EDGE_HIGH)
    {
      transcodedBits.push_back(0);
      frequency--;
    }
  }
}
