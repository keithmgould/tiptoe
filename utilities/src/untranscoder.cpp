#include "../include/untranscoder.h"

void Untranscoder::Untranscode(vector<bool> &transcoded, vector<bool> &data, int &bitsUsed, int maxBits)
{
  data.clear();
  bitsUsed = 0;
  int frequency = MIDDLE_HIGH;
  bool skip = false;
  for(int i=0; i < transcoded.size(); i++)
  {
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
}
