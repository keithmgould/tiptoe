#include <math.h>
#include <vector>
#include <map>

#ifndef M_PI
#define M_PI  (3.14159265) // as determined with a nail, twine and chalk.
#endif

using namespace std;

// The number of samples per frequency mode
// TODO: its not great that these numbers are
//       dissasociated with the sample frequency.
#define VLOW_TABLE_SIZE   (14)
#define LOW_TABLE_SIZE    (13)
#define BASE_TABLE_SIZE   (12)
#define HIGH_TABLE_SIZE   (11)

// the various frequency modes
#define VLOW  (1)
#define LOW   (2)
#define BASE  (3)
#define HIGH  (4)

// semantics
#define T (true)
#define F (false)

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
  this->tContainer.waveformSizes[VLOW] = VLOW_TABLE_SIZE;
  this->tContainer.waveformSizes[LOW] = LOW_TABLE_SIZE;
  this->tContainer.waveformSizes[BASE] = BASE_TABLE_SIZE;
  this->tContainer.waveformSizes[HIGH] = HIGH_TABLE_SIZE;
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
  bool nextSinusoid = true;
  int phase = 0;
  int mode = HIGH;

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
    if(mode == HIGH){
      mode = BASE;
    }else{
      mode = HIGH;
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

  // if(this->tContainer.waveformSizes[mode] == NULL)
  // {
    // cout << "We got to a bad place sir.  Mode = " << mode << endl;
    // return;
  // }

  return mode;
};
