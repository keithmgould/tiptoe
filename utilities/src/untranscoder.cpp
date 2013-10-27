#include "../include/untranscoder.h"

/*
  Untranscode Bits:

  The preamble leaves us at the Middle High frequency.
  The frequency variable is used to keep track of which frequency
  we are at in case we reach an "edge" frequency and need to
  "recover".  On the receiving side, frequency will be kept track
  of so when an edge bit is reached, it will know to throw
  out the next "recovery bit"

  bitsUsed tells the caller how many transcoded bits were used to
  "unveil" the raw bits.

  maxBits lets the untranscoder know to stop early.  This is needed
  because sometimes the caller only wants a certain amount of the raw
  data back (untranscoded).

*/
void Untranscoder::Untranscode(vector<bool> &transcoded, vector<bool> &data, int &bitsUsed, int maxBits)
{
  data.clear();
  bitsUsed = 0;
  vector<int> frequencies;
  int frequency = MIDDLE_HIGH;
  bool skip = false;
  for(int i=0; i < transcoded.size(); i++)
  {
    frequencies.push_back(frequency);
    bitsUsed++;

    if(skip)
    {
      skip = false;
    }else{
      data.push_back(transcoded.at(i));
    }

    if(transcoded.at(i) == 1)
    {
      frequency++;
      if(frequency == EDGE_HIGH) { skip = true; }
    }else{
      frequency--;
      if(frequency == EDGE_LOW) { skip = true; }
    }

    if(maxBits > 0 && ((int) data.size() == (int) maxBits))
    {
      if(skip == true) { bitsUsed++; }
      break;
    }
  }
  if(DEBUG_MODE > 1)
  {
    Printer::print("frequencies", frequencies);
  }
}
