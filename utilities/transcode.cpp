#include <vector>
#include <iostream>
using namespace std;

/* Transcode
 *
 */
class Transcode {
  public:

  static void Perform(vector<bool> &rawBits, vector<bool> &transcodedBits)
  {
    // First place the preamble.
    // Takes us from PREAMBLE_LOW to MIDDLE_HIGH
    transcodedBits.push_back(1);
    transcodedBits.push_back(1);
    transcodedBits.push_back(1);
    transcodedBits.push_back(1);
    transcodedBits.push_back(0);

    // The preamble leaves us at the Middle High frequency.
    // The frequency variable is used to keep track of which frequency
    // we are at in case we reach an "edge" frequency and need to
    // "recover"
    int frequency = MIDDLE_HIGH;

    //
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
};
