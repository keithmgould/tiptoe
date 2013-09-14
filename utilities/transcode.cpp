#include <bitset>
#include <vector>
#include <iostream>
using namespace std;

/* Transcode
 *
 */
class Transcode {
  public:

  static void Perform(unsigned char inputBytes[], vector<bool> &transcodedBits, int byteCount)
  {
    // First place the preamble.
    // Takes us from PREAMBLE_LOW to MIDDLE_HIGH
    transcodedBits.push_back(T);
    transcodedBits.push_back(T);
    transcodedBits.push_back(T);
    transcodedBits.push_back(T);
    transcodedBits.push_back(F);

    int frequency = MIDDLE_HIGH;
    for(int i = 0; i < byteCount; i++)
    {
      bitset<8> bits = bitset<8>(inputBytes[i]);
      for(int j=7; j >=0; j--)
      {
        // Here we have a direct mapping between input and output
        if(DEBUG_MODE > 0) { cout << bits[j]; }
        if(bits[j] == 1)
        {
          transcodedBits.push_back(T);
          frequency++;
        }else{
          transcodedBits.push_back(F);
          frequency--;
        }

        // If we ended up on an "edge" frequency,
        // recover by going in the other direction.
        // This bit will be thrown out on the other side.
        if(frequency == EDGE_LOW)
        {
          transcodedBits.push_back(T);
          frequency++;
        }
        if(frequency == EDGE_HIGH)
        {
          transcodedBits.push_back(F);
          frequency--;
        }
      }
    }
    if (DEBUG_MODE > 0) { cout << endl; }
    if (DEBUG_MODE > 1 )
    {
      cout << "trn:";
      for(int i = 0; i< transcodedBits.size(); i++)
      {
        cout << transcodedBits.at(i);
      }
      cout << endl << endl;
    }
  }
};
