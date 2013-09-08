#include <math.h>
#include <vector>
#include <map>

using namespace std;

// The number of samples per frequency mode
// TODO: its not great that these numbers are
//       dissasociated with the sample frequency.

// local input audio samples are of type short
typedef short SAMPLE;

// holds the tContainer
typedef struct
{
  map<int, vector<SAMPLE> > waveforms;
  map<int, int> waveformSizes;
}
transmitContainer;

class Transmitter {
  vector<bool> transcodedBits;
  vector<bool>::iterator bitIterator;
  int framesPerBuffer;
  void buildWaveforms();
  int determineNextMode(int mode);
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
  this->tContainer.waveformSizes[PREAMBLE_LOW] = PREAMBLE_LOW_TABLE_SIZE;
  this->tContainer.waveformSizes[EDGE_LOW] = EDGE_LOW_TABLE_SIZE;
  this->tContainer.waveformSizes[MIDDLE_LOW] = MIDDLE_LOW_TABLE_SIZE;
  this->tContainer.waveformSizes[MIDDLE_HIGH] = MIDDLE_HIGH_TABLE_SIZE;
  this->tContainer.waveformSizes[EDGE_HIGH] = EDGE_HIGH_TABLE_SIZE;
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

  for(int i=0; i < this->framesPerBuffer; i++ )
  {
    if(nextSinusoid)
    {
      mode = this->determineNextMode(mode);
      nextSinusoid = false;
      phase = 0;
    }

    *out++ = this->tContainer.waveforms[mode].at(phase++);
    if(phase >= this->tContainer.waveformSizes[mode]) { nextSinusoid = true;}
  }
}

/*
   Build Wave Forms
   Builds the waveforms used to generate sound.
   This is done using the standard definition of sine, and then converted
   into a type short via multiplication of 32767.
*/
void Transmitter::buildWaveforms()
{
  int i;

  map<int,int>::iterator it = this->tContainer.waveformSizes.begin();
  SAMPLE sample;

  // for each waveform size
  for(;it != this->tContainer.waveformSizes.end(); it++)
  {
    for( i=0; i< it->second; i++ )
    {
      sample = (SAMPLE) (sin( ((double)i/(double) it->second) * M_PI * 2.0) * 32767);
      this->tContainer.waveforms[it->first].push_back(sample);
    }
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
int Transmitter::determineNextMode(int mode)
{
  // if we are finished sending data, use alternating
  // ones and zeros as filler.  Otherwise send the
  // data
  if( this->bitIterator == this->transcodedBits.end() )
  {
    if(mode == MIDDLE_HIGH){
      mode = MIDDLE_LOW;
    }else{
      mode = MIDDLE_HIGH;
    }
  }else{
    if( *this->bitIterator == F)
    {
      mode -= 1;
    }else{
      mode += 1;
    }
    this->bitIterator++;
  }

  // if(!this->tContainer.waveformSizes.find(mode))
  // {
    // cout << "We got to a bad place sir.  Mode = " << mode << endl;
    // return;
  // }

  return mode;
};
