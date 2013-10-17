#include "../include/transcoder.h"
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
void Transcoder::Transcode(vector<bool> &rawBits, vector<bool> &transcodedBits)
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
