#include "../include/transcode.h"
/*
  Transcoding will create the "packet" sent across the wire/air.
  The packet is 107 bits and structured as follows:

  bits - usage
  5    - preamble
  5    - bufferCounter
  90   - transcodedBits.
  7    - 7 bit parity hamming error correctig code, based on bufferCounter and transcodedBits
*/

/*
  Arguments:

  rawBits: the incoming raw data.  48 bits long.
  transcodedBits: the outgoing transcoded bits.  < 107 bits long.
  bufferCounter: the 5 bit integer used to ensure both sides
                 stay in sync.
*/
void Transcode::perform(vector<bool> &rawBits, vector<bool> &transcodedBits)
{
  add_preamble_bits(transcodedBits);
  add_transcoded_bits(rawBits, transcodedBits);
  add_buffer_bits(transcodedBits);
  add_hamming_bits(transcodedBits);

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

void Transcode::add_preamble_bits(vector<bool> &transcodedBits)
{
  transcodedBits.push_back(1);
  transcodedBits.push_back(1);
  transcodedBits.push_back(1);
  transcodedBits.push_back(1);
  transcodedBits.push_back(0);
}

/*
 if the transcoding process produced less than 90
 bits (very very likely) then add alternating 1s
 and 0s until there are 90 bits.

 using 95 as the counter because the preamble (5 bits)
 has already been added in.
 */
void Transcode::add_buffer_bits(vector<bool> &transcodedBits)
{
  int val = 0;
  while(transcodedBits.size() < 95)
  {
    transcodedBits.push_back(val);
    val = (val + 1) % 2;
  }
}

void Transcode::add_hamming_bits(vector<bool> &transcodedBits)
{
  vector<bool> foo;

  foo.push_back(1);
  foo.push_back(0);
  foo.push_back(1);
  foo.push_back(0);
  vector<bool> encodedBits;

  hamming.encode(foo, encodedBits);
}
/*
  Add Transcoded Bits:

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
void Transcode::add_transcoded_bits(vector<bool> &rawBits, vector<bool> &transcodedBits)
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

