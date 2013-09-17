/* Extract
 *
 * Extract's job is the opposite of Transcode's job (and then some.)
 *
 * A Frame sent over the wire has three parts:
 *   1) The preamble: 11110
 *   2) The Data
 *   3) 1s and 0s alternating until the frame is complete (garbage)
 *
 * Extract's job is to extract the data
 *
 * PSEUDO CODE:
 * IF raw bits from previous buffer?
 *    add to current buffer's raw bits
 *
 * IF found 2 preambles in raw bits
 *    extract data inbetween preambles
 *    store 2nd preamble and following raw bits for next buffer
 * ELSEIF found 1 preamble in raw bits
 *    store preamble and following raw bits for next buffer
 * ELSE
 *    // this means the preamble was split in two, so no
 *    // preamble was found
 *    store everything for next buffer
 */

#include <vector>
#include <utility>
using namespace std;

class Extract
{
  public:
  vector<bool> totalRawBits;

  Extract (vector<bool> &newRawBits, vector<bool> &previousRawBits); // constructor
  void perform(vector<bool> &extractedBits, vector<bool> &remainingRawBits);
  pair <int, int> findPreambles();
};

/*
 * The Constructor
 */
Extract::Extract (vector<bool> &newRawBits, vector<bool> &previousRawBits)
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

void Extract::perform(vector<bool> &extractedBits, vector<bool> &remainingBits)
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

pair <int, int> Extract::findPreambles()
{
  pair <int, int> indexes (-1, -1);
  for(int i = 4; i < this->totalRawBits.size(); i++)
  {
    if(this->totalRawBits.at(i-4) == T &&
       this->totalRawBits.at(i-3) == T &&
       this->totalRawBits.at(i-2) == T &&
       this->totalRawBits.at(i-1) == T &&
       this->totalRawBits.at(i)   == F)
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
