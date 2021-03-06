#include "../include/extractor.h"
/* Extract

  A Packet sent over the wire has three parts:
    1) The preamble: 11110
    2) The Data
    3) 1s and 0s alternating until the pcaket is complete

  Extract's job is to extract the data.  However, we receive buffers
  that almost always chop packets in half, so they need to be put
  back together again.  So we pay attention to what was in the
  previous buffer, as well as the current buffer.

  PSEUDO CODE:
  IF raw bits from previous buffer?
     add to current buffer's raw bits

  IF found 2 preambles in raw bits
     extract data inbetween preambles
     store 2nd preamble and following raw bits for next buffer
  ELSEIF found 1 preamble in raw bits
     store preamble and following raw bits for next buffer
  ELSE
     // this means the preamble was split in two, so no
     // preamble was found
     store everything for next buffer
 */



/*
 * The Constructor
 */
Extractor::Extractor (vector<bool> &newRawBits, vector<bool> &previousRawBits)
{
  if(DEBUG_MODE > 1)
  {
    cout << "prv:";
    for(int i = 0; i< previousRawBits.size(); i++)
    {
      cout << previousRawBits.at(i);
    }
    cout << endl << "new:";
    for(int i = 0; i< newRawBits.size(); i++)
    {
      cout << newRawBits.at(i);
    }
    cout << endl;
  }
  this->totalRawBits = previousRawBits;
  this->totalRawBits.insert(this->totalRawBits.end(), newRawBits.begin(), newRawBits.end());
}

void Extractor::perform(vector<bool> &extractedBits, vector<bool> &remainingBits)
{
  extractedBits.clear();
  remainingBits.clear();
  pair <int, int> preambleIndexes = findPreambles();
  vector<bool>::iterator firstIndexAt;
  vector<bool>::iterator secondIndexAt;
  if(preambleIndexes.first >= 0)
  {
    if(preambleIndexes.second >= 0)
    {
      firstIndexAt = this->totalRawBits.begin() + preambleIndexes.first;
      secondIndexAt = this->totalRawBits.begin() + preambleIndexes.second;
      extractedBits.assign(firstIndexAt+5, secondIndexAt);
      remainingBits.assign(secondIndexAt, this->totalRawBits.end() );
    }else{
      firstIndexAt = this->totalRawBits.begin() + preambleIndexes.first;
      remainingBits.assign(firstIndexAt, this->totalRawBits.end() );
    }
  }else{
    remainingBits = this->totalRawBits;
  }
}

pair <int, int> Extractor::findPreambles()
{
  pair <int, int> indexes (-1, -1);
  for(int i = 4; i < this->totalRawBits.size(); i++)
  {
    if(this->totalRawBits.at(i-4) == 1 &&
       this->totalRawBits.at(i-3) == 1 &&
       this->totalRawBits.at(i-2) == 1 &&
       this->totalRawBits.at(i-1) == 1 &&
       this->totalRawBits.at(i)   == 0)
    {
      if(indexes.first == -1)
      {
        indexes.first = i-4;
      }else{
        indexes.second = i-4;
        break;
      }
    }
  }
  return indexes;
}
