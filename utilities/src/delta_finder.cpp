#include "../include/delta_finder.h"

/*
  Given samples describing a sinudoid that oscilates about zero, determine
  the time between each zero-crossing (in the upward direction).

  This is used by the demodulator.

  float inputSamples: the raw float input samples.
  int sampleCount: the number of samples per buffer
  int sampleRate: the sample rate (samples per second)
  vector Deltas: holds the float deltas between upwards zero crossings
  float timeAfterLastBuffersLastCrossing: how much time between the last crossing and the end of the last buffer
*/


void DeltaFinder::Perform(vector<float>& inputSamples, int sampleCount, int sampleRate, vector<float>& deltas, float *timeAfterLastBuffersLastCrossing)
{
  float delta = 1 / (float) sampleRate; // the time between two samples
  float crossingTime;
  float lastCrossingTime = -1;
  float sampleAbsVal;
  for(int i=1;i<sampleCount;i++)
  {
    // if we have crossed zero going from the last sample upwards to the current sample
    if(inputSamples.at(i-1) < 0 && inputSamples.at(i) > 0)
    {
      sampleAbsVal = std::abs(inputSamples.at(i-1));
      // This is calculated using basic geometry.  The length of the tangent
      // of the smaller triangle is proportional to the length of the tangent
      // of the larger triangle
      crossingTime = delta  * sampleAbsVal / (inputSamples.at(i) + sampleAbsVal);
      crossingTime = crossingTime + delta * (i - 1);
      // if we have just found our first crossing then we want to determine
      // the delta between the previous buffer's last crossing and this
      // buffer's first crossing
      if(*timeAfterLastBuffersLastCrossing > 0)
      {
        lastCrossingTime = 0 - *timeAfterLastBuffersLastCrossing;
        if(DEBUG_MODE > 2)
        {
          cout << "first delta info: i=" << i << ", ";
          cout << "talblc= " << *timeAfterLastBuffersLastCrossing << ", ";
          cout << "ct= " << crossingTime << ", ";
          cout << crossingTime - lastCrossingTime << endl;
        }
        *timeAfterLastBuffersLastCrossing = -1; // make sure this condition is met only once
      }
      deltas.push_back(crossingTime - lastCrossingTime);
      lastCrossingTime = crossingTime;
    }
  }
  if(DEBUG_MODE > 2)
  {
    cout << "first 5 samples: ";
    for(int i=0;i < 5;i++)
    {
      cout << inputSamples.at(i) << ", ";
    }
    cout << endl;
    cout << "last 5 samples: ";
    for(int i=6;i>=1;i--)
    {
      cout << inputSamples.at(sampleCount - i) << ", ";
    }
    cout << endl;
  }
  // we subtract the last delta because each buffer receives the last sample from the prev buffer
  *timeAfterLastBuffersLastCrossing = ( (float) sampleCount / (float) sampleRate ) - lastCrossingTime - delta;
}
