#include <cmath>
#include <vector>
/*
  Given samples describing a sinudoid that oscilates about zero, determine
  the time between each zero-crossing (in the upward direction).

  This is used by the demodulator.

  float inputSamples: the raw float input samples.
  int sampleCount: the number of samples per buffer
  int sampleRate: the sample rate (samples per second)
  vector Deltas: holds the float deltas between upwards zero crossings
  float deltaAfterLastCrossing: how much time between the last crossing and the end of the last buffer
*/

using namespace std;

class DeltaFinder
{
  public:
  static void Perform(float inputSamples[], int sampleCount, int sampleRate, vector<float>& deltas, float *deltaAfterLastCrossing)
  {
    float delta = 1 / (float) sampleRate; // the time between two samples
    float crossingTime;
    float lastCrossingTime = -1;
    float sampleAbsVal;

    for(int i=1;i<sampleCount;i++)
    {
      // if we have crossed zero going from the last sample upwards to the current sample
      if(inputSamples[i-1] < 0 && inputSamples[i] > 0)
      {
        sampleAbsVal = std::abs(inputSamples[i-1]);
        // This is calculated using basic geometry.  The length of the tangent
        // of the smaller triangle is proportional to the length of the tangent
        // of the larger triangle
        crossingTime = delta  * sampleAbsVal / (inputSamples[i] + sampleAbsVal);
        crossingTime = crossingTime + delta * (i - 1);
        // if we have just found our first crossing and want to determine the delta between the
        // previous buffer's last crossing and this buffer's first crossing
        if(*deltaAfterLastCrossing > 0)
        {
          lastCrossingTime = 0 - *deltaAfterLastCrossing;
          *deltaAfterLastCrossing = -1; // make sure this condition is met only once
        }
        deltas.push_back(crossingTime - lastCrossingTime);
        lastCrossingTime = crossingTime;
      }
    }
    *deltaAfterLastCrossing = ( (float) sampleCount / (float) sampleRate ) - lastCrossingTime;
  }
};
