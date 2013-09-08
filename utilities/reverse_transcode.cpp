#include <vector>

using namespace std;

/* Reverse Transcode
 *
 * Reverse Transcode takes transcoded data and reverses the transcoding,
 * leaving the actual bits originally generated on the other side of the wire.
 */

class ReverseTranscode
{
  public:
  vector<bool> extractedBits;
  ReverseTranscode(vector<bool> &extractedBits);
  void perform(vector<bool> &dataBits);
};

ReverseTranscode::ReverseTranscode(vector<bool> &extractedBits)
{
  this->extractedBits = extractedBits;
}

void ReverseTranscode::perform(vector<bool> &dataBits)
{
  dataBits.clear();
  vector<bool>::iterator it = this->extractedBits.begin();
  int frequency = MIDDLE_HIGH;
  bool skip = false;
  for(; it != this->extractedBits.end(); it++)
  {
    if(skip == false)
    {
      dataBits.push_back(*it);
    }else{
      skip = false;
    }

    if(*it == T)
    {
      frequency++;
      if(frequency == EDGE_HIGH) { skip = true; }
    }else{
      frequency--;
      if(frequency == EDGE_LOW) { skip = true; }
    }
  }
}

