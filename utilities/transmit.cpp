#include <math.h>

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

// local input audio samples are of type short
typedef short SAMPLE;

// holds the waveforms
typedef struct
{
  SAMPLE base_sine[BASE_TABLE_SIZE];
  SAMPLE high_sine[HIGH_TABLE_SIZE];
  SAMPLE low_sine[LOW_TABLE_SIZE];
  SAMPLE vlow_sine[VLOW_TABLE_SIZE];
}
waveformContainer;

class Transmitter {
  vector<bool> transcodedBits;
  vector<bool>::iterator bitIterator;
  int framesPerBuffer;
  void buildWaveforms();
  int determineNextMode(int mode);
  public:
  void emitSound(short *out );
  void setBits(vector<bool> &transcodedBits);
  waveformContainer waveforms;
  Transmitter (int framesPerBuffer);
};

// Constructor
Transmitter::Transmitter (int framesPerBuffer)
{
  this->framesPerBuffer = framesPerBuffer;
  buildWaveforms();
}

// resets the transmitter with the new data to send
void Transmitter::setBits ( vector<bool> &transcodedBits )
{
  // TODO: do we need to clear the vector here?
  this->transcodedBits = transcodedBits;
  this->bitIterator = this->transcodedBits.begin();
  // this->transcodedBits.push_back(true);
}

/*
  Emit Sound
  This function places the proper sinusoids in the output buffer
  TODO: Refactor
*/
void Transmitter::emitSound( short *out )
{
  bool nextSinusoid = true;
  int phase = 0;
  int mode = BASE;
  int preamble = 0;
  int bitIterator = 0;

  for(int i=0; i < this->framesPerBuffer; i++ )
  {
    if(nextSinusoid)
    {
      mode = this->determineNextMode(mode);
      nextSinusoid = false;
      phase = 0;
    }
    if (mode == HIGH){
      *out++ = this->waveforms.high_sine[phase];
      phase += 1;
      if(phase >= HIGH_TABLE_SIZE) { nextSinusoid = true;}
    }else if(mode == BASE){
      *out++ = this->waveforms.base_sine[phase];
      phase += 1;
      if(phase >= BASE_TABLE_SIZE) { nextSinusoid = true;}
    }else if(mode == LOW){
      *out++ = this->waveforms.low_sine[phase];
      phase += 1;
      if(phase >= LOW_TABLE_SIZE) { nextSinusoid = true;}
    }else if(mode == VLOW){
      *out++ = this->waveforms.vlow_sine[phase];
      phase += 1;
      if(phase >= VLOW_TABLE_SIZE) { nextSinusoid = true;}
    }else{
      printf("we got to a bad place sir.\n");
      return;
    }
  }
}

/*
   Build Wave Forms
   Builds the waveforms used to generate sound.
   This is done using the standard definition of sine, and then converted
   into a type short with the multiplication of 32767.
   TODO: refactor
*/
void Transmitter::buildWaveforms()
{
  int i;
  for( i=0; i<BASE_TABLE_SIZE; i++ )
  {
    this->waveforms.base_sine[i] = (SAMPLE) (sin( ((double)i/(double)BASE_TABLE_SIZE) * M_PI * 2.0) * 32767);
  }

  for( i=0; i<HIGH_TABLE_SIZE; i++ )
  {
    this->waveforms.high_sine[i] = (SAMPLE) (sin( ((double)i/(double)HIGH_TABLE_SIZE) * M_PI * 2.0) * 32767);
  }

  for( i=0; i<LOW_TABLE_SIZE; i++ )
  {
    this->waveforms.low_sine[i] = (SAMPLE) (sin( ((double)i/(double)LOW_TABLE_SIZE) * M_PI * 2.0) * 32767);
  }

  for( i=0; i<VLOW_TABLE_SIZE; i++ )
  {
    this->waveforms.vlow_sine[i] = (SAMPLE) (sin( ((double)i/(double)VLOW_TABLE_SIZE) * M_PI * 2.0) * 32767);
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
  if( this->bitIterator == this.transcodedBits.end() )
  {
    if(mode == HIGH){
      mode = BASE;
    }else{
      mode = HIGH;
    }
  } else{
    if( this->bitIterator == 0)
    {
      mode -= 1;
    }else{
      mode += 1;
    }
    this->bitIterator++;
  }
  return mode;
};
