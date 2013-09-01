#include <vector>
/*
 Demodulate is based off of the demodulator described in the Hermes Paper
 by Aditya Dhananjay of NYU.

  Pseudo Code:
  for each sinusoid in the input sound signal
    fcurr = current sinusoid frequency
    fprev = previous sinusoid frequency
    if fcurr > fprev
      output 1
    else
      output 0
    end
  end

  We are receiving the time deltas, which means
  the higher the delta, the lower the frequency

  Arguments:
    deltas: time deltas between sinusoids
    outputBits: pointer to the demodulated data array
*/
using namespace std;

class Demodulate
{
  public:
    static void Perform(vector<float>& deltas, vector<bool>& outputBits)
    {
      bool result;
      for(int i = 1; i != deltas.size(); i++)
      {
        result = (deltas.at(i) < deltas.at(i-1));
        outputBits.push_back(result);
      }
    }
};
