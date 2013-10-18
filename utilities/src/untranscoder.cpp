#include "../include/untranscoder.h"

void Untranscoder::Untranscode(vector<bool> &transcoded, vector<bool> &data, int &bitsUsed, int maxBits)
{
  data.clear();
  bitsUsed = 0;
  int frequency = MIDDLE_HIGH;
  bool skip = false;
  if(DEBUG_MODE > 1) { cout << "ext:"; }
  for(int i=0; i < transcoded.size(); i++)
  {
    bitsUsed++;
    if(DEBUG_MODE > 1) { cout << transcoded.at(i); }
    if(skip)
    {
      skip = false;
    }else{
      data.push_back(transcoded.at(i));
      if(maxBits > 0 && data.size() == maxBits)
      {
        bitsUsed++;
        break;
      }
    }

    if(transcoded.at(i) == 1)
    {
      frequency++;
      if(frequency == EDGE_HIGH) { skip = true; }
    }else{
      frequency--;
      if(frequency == EDGE_LOW) { skip = true; }
    }
  }
  if(DEBUG_MODE > 1) { cout << endl << endl; }
}
