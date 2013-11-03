#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <math.h>
#include <vector>
#include <map>
#include <sys/time.h>

using namespace std;

// used for modulating the amplitude to prevent
// voice detection from thinking the audio
// is just background noise.
const float SMALLER_AMPLITUDE = 0.60;

// holds the tContainer
typedef struct
{
  map<int, vector<short> > waveforms;
  map<int, double> waveformSizes;
}
transmitContainer;

class Transmitter {
  vector<bool> transcodedBits;
  vector<bool>::iterator bitIterator;
  int framesPerBuffer;
  bool amplitudeMode;
  void buildWaveforms();
  int determineNextMode(int mode, int onFrame);
  int low_frequency;
  int high_frequency;
  public:
  void emitSound(short *out );
  void setBits(vector<bool> &transcodedBits);
  transmitContainer tContainer;
  Transmitter (int framesPerBuffer);
};

#endif
