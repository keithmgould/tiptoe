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
  public:
  void emitSound(short *out );
  void setBits(vector<bool> &transcodedBits);
  transmitContainer tContainer;
  Transmitter (int framesPerBuffer);
};

// Constructor
Transmitter::Transmitter (int framesPerBuffer)
{
  this->framesPerBuffer = framesPerBuffer;
  this->tContainer.waveformSizes[PREAMBLE_LOW] = (double) SAMPLE_RATE / (double) PREAMBLE_LOW_FREQUENCY;
  this->tContainer.waveformSizes[EDGE_LOW] = (double) SAMPLE_RATE / (double) EDGE_LOW_FREQUENCY;
  this->tContainer.waveformSizes[MIDDLE_LOW] = (double) SAMPLE_RATE / (double) MIDDLE_LOW_FREQUENCY;
  this->tContainer.waveformSizes[MIDDLE_HIGH] = (double) SAMPLE_RATE / (double) MIDDLE_HIGH_FREQUENCY;
  this->tContainer.waveformSizes[EDGE_HIGH] = (double) SAMPLE_RATE / (double) EDGE_HIGH_FREQUENCY;
  buildWaveforms();
}

// resets the transmitter with the new data to send
void Transmitter::setBits ( vector<bool> &transcodedBits )
{
  this->transcodedBits = transcodedBits;
  this->bitIterator = this->transcodedBits.begin();
}

/*
  Emit Sound
  This function places the proper sinusoids in the output buffer
*/
void Transmitter::emitSound( short *out )
{
  bool nextSinusoid = false;
  int phase = 0;
  int mode = PREAMBLE_LOW;
  float amp;
  float fpb;
  fpb = this->framesPerBuffer;
  for(int i=0; i < fpb; i++ )
  {
    if(nextSinusoid)
    {
      mode = this->determineNextMode(mode, i);
      nextSinusoid = false;
      phase = 0;
    }
    amp = 0.5;
    if( i <= (fpb / 2))
    {
      amp += i/fpb;
    }else{
      amp += (fpb - i) / fpb;
    }
    // cout << amp << ", ";
    if(this->amplitudeMode)
    {
      *out++ = amp * this->tContainer.waveforms[mode + 100].at(phase++);
    }else{
      *out++ = amp * this->tContainer.waveforms[mode].at(phase++);
    }
    if(phase >= this->tContainer.waveformSizes[mode]) { nextSinusoid = true;}
  }
}

/*
   Build Wave Forms
   Builds the waveforms used to generate sound.
   This is done using the standard definition of sine, and then converted
   into a type short via multiplication of 32767.
   We also find the amplitude at 70%.  This is because every half second we
   switch from one amplitude to another to defeat Voice Activity Detection (VAD).
*/

void Transmitter::buildWaveforms()
{
  int i;

  map<int,double>::iterator it = this->tContainer.waveformSizes.begin();
  short sample;

  // for each waveform size
  for(;it != this->tContainer.waveformSizes.end(); it++)
  {
    cout << "wav " << it->first << ": ";
    for( i=0; i< it->second; i++ )
    {
      // Full amplitude waveforms
      sample = (short) (sin( ((double)i/it->second) * M_PI * 2.0) * 32767);
      cout << sample << ", ";
      this->tContainer.waveforms[it->first].push_back(sample);

      // 70% amplitude waveforms.
      sample = sample * SMALLER_AMPLITUDE;
      this->tContainer.waveforms[it->first + 100].push_back(sample);
    }
    cout << endl;
  }
}

/*
  Determine Next Mode.

  modulation is achieved using the IncDec algorithm found in the
  Hermes paper.

  Once all the data is modulated, the function returns zeros and ones until the output
  buffer is full.

  TODO: Refactor
 */
int Transmitter::determineNextMode(int mode, int onFrame)
{
  // get us back to pre-amplitude-adjustd mode

  // if we are finished sending data, use alternating
  // ones and zeros as filler.  Otherwise send the
  // data
  if( this->bitIterator == this->transcodedBits.end() )
  {
    int framesLeft = this->framesPerBuffer - onFrame;
    if( framesLeft < 20 )
    {
      // cout << framesLeft << ", ";
      // ok, we only have 8 frames left.  All existing waveforms are larger than 8 frames.
      // So we have to engineer a new shiny waveform with the remaining frames.
      short sample;
      this->tContainer.waveforms[50000].clear();
      this->tContainer.waveformSizes[50000] = framesLeft;
      for( int i=0; i< framesLeft; i++ )
      {
        sample = (short) (sin( ((double)i/(double) framesLeft) * M_PI * 2.0) * 32767);
        this->tContainer.waveforms[50000].push_back(sample);
        // 70% amplitude waveforms.
        sample = sample * SMALLER_AMPLITUDE;
        this->tContainer.waveforms[50100].push_back(sample);
      }
      mode = 50000;
    }else{
      switch(mode)
      {
        case MIDDLE_HIGH:
          mode = MIDDLE_LOW; break;
        case MIDDLE_LOW:
          mode = MIDDLE_HIGH; break;
        case EDGE_LOW:
          mode = MIDDLE_LOW; break;
        case EDGE_HIGH:
          mode = MIDDLE_HIGH; break;
      }
    }
  }else{
    if( *this->bitIterator == 0)
    {
      mode -= 1;
    }else{
      mode += 1;
    }
    this->bitIterator++;
  }

  // if we are in the second half of a second, add 100 to the mode
  // to adjust the amplitude.
  timeval time;
  gettimeofday(&time, NULL);
  long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
  millis = millis % 1000;

  amplitudeMode = (millis < 250) || (millis > 500 && millis < 750);
  return mode;
};
