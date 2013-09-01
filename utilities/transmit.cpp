#include <math.h>

#ifndef M_PI
#define M_PI  (3.14159265) // as determined with a nail, twine and chalk.
#endif

using namespace std;

// The number of samples per frequency mode
#define VLOW_TABLE_SIZE   (14)
#define LOW_TABLE_SIZE    (13)
#define BASE_TABLE_SIZE   (12)
#define HIGH_TABLE_SIZE   (11)

// the various frequency modes.  VLOW is only used in
// the preamble
#define VLOW  (1)
#define LOW   (2)
#define BASE  (3)
#define HIGH  (4)

// audio samples are of type short;
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
  bool * transcodedBits;
  int framesPerBuffer;
  void buildWaveforms();
  void determineNextMode(int *mode, int *preamble, bool transcodedBits[], int *bitIterator);
  public:
  void emitSound(short *out );
  void setBits(bool transcodedBits[] );
  waveformContainer waveforms;
  Transmitter (int framesPerBuffer);
};

// Constructor
Transmitter::Transmitter (int framesPerBuffer)
{
  this->framesPerBuffer = framesPerBuffer;
  buildWaveforms();
}

void Transmitter::setBits ( bool transcodedBits[] )
{
  this->transcodedBits = transcodedBits;
}

/*
  Emit Sound
  This function places the proper sinusoids in the output buffer
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
      this->determineNextMode(&mode, &preamble, transcodedBits, &bitIterator);
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
*/
void Transmitter::buildWaveforms()
{
  int i;
  /* initialise sinusoidal wavetables */
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
  Determine Next Node.
  Each buffer begins with preamble consisting of 4 sinusoids,
  one of which is not possible to use during normal
  modulation (this is the VLOW frequency).  The VLOW sinusoid allows for the preamble
  to emit three consecutive ones, which is not possible during norming modulation.

  After the preamble, modulation is achieved using the IncDec algorithm found in the
  Hermes paper.
 */
void Transmitter::determineNextMode(int *mode, int *preamble, bool transcodedBits[], int *bitIterator)
{
  if (*preamble <= 4)
  {
    switch (*preamble){
      case 0:
        // cout << endl << endl;
        *mode = VLOW;
        break;
      case 1:
        *mode = LOW;
        break;
      case 2:
        *mode = BASE;
        break;
      case 3:
        *mode = HIGH;
        break;
      case 4:
        *mode = BASE;
        break;
    }
    *preamble += 1;
  }else{
    if(*bitIterator >= 96)
    {
      if(*mode == HIGH){
        *mode = BASE;
      }else{
        *mode = HIGH;
      }
    } else{
      // IncDec Algorithm #1 from Hermes paper.
      if(transcodedBits[*bitIterator] == 0)
      {
        *mode -= 1;
      }else{
        *mode += 1;
      }
      *bitIterator += 1;
    }
  }
  // cout << *mode;
};



// int determineNeededFrameCount(bool transcodedBits[])
// {
  // int phase = 0;
  // int mode = BASE;
  // int totalFrames = 0;
  // int lowCount = 0;
  // int highCount = 0;
  // int baseCount = 0;
  // int bitIterator = 0;

  // // Count how many frames are needed to modulate the data
  // for(bitIterator = 0; bitIterator< 96; bitIterator++)
  // {
    // if(transcodedBits[bitIterator] == 0)
    // {
      // mode -= 1;
    // }else{
      // mode += 1;
    // }
    // //--------------------------------
    // if(mode == LOW)
    // {
      // lowCount += 1;
      // totalFrames += LOW_TABLE_SIZE;
    // }else if(mode == BASE)
    // {
      // baseCount += 1;
      // totalFrames += BASE_TABLE_SIZE;
    // }else if(mode == HIGH){
      // highCount += 1;
      // totalFrames += HIGH_TABLE_SIZE;
    // }else{
      // cout << endl << "mode is out of range.  It is now: " << mode << endl;
      // return 1;
    // }
  // }

  // // frames for preamble
  // totalFrames += VLOW_TABLE_SIZE + LOW_TABLE_SIZE + (2 * BASE_TABLE_SIZE) + HIGH_TABLE_SIZE;

  // // int neededFrames =  FRAMES_PER_BUFFER - totalFrames;
  // // printf("totalFrames: %d\n", totalFrames);
  // // printf("lowCount: %d\n", lowCount);
  // // printf("baseCount: %d\n", baseCount);
  // // printf("highCount: %d\n", highCount);
  // // printf("neededFrames: %d\n", neededFrames);
  // return totalFrames;
// };
