#include "../include/transcoder.h"
/*
  Transcode Bits:

  The preamble leaves us at the Middle High frequency.
  The frequency variable is used to keep track of which frequency
  we are at in case we reach an "edge" frequency and need to
  "recover".  On the receiving side, frequency will be kept track
  of so when an edge bit is reached, it will know to throw
  out the next "recovery bit"

*/
void Transcoder::Transcode(vector<bool> &rawBits, vector<bool> &transcodedBits)
{
  int frequency = MIDDLE_HIGH;

  for(int i = 0; i < rawBits.size(); i++)
  {
    // Here we have a direct mapping between input and output
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
