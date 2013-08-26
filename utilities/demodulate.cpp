#include <cmath>
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

  Arguments:
    deltas: time deltas between sinusoids
    outputBits: pointer to the demodulated data array
*/
class Demodulate
{
  public:
    static void Perform(vector<float>& deltas, vector<bool>& outputBits)
    {
      for(int i = 1; i != deltas.size(); i++)
      {
        outputBits.push_back( deltas.at(i) <= deltas.at(i-1) );
      }
    }
};
