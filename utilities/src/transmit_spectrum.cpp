#include <math.h>
#include <vector>
#include <map>
#include <sys/time.h>

using namespace std;

// The number of samples per frequency
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

class SpectrumTransmitter {
  vector<bool> transcodedBits;
  vector<bool>::iterator bitIterator;
  bool amplitudeMode;
  int frequency;
  void buildWaveforms();
  void determineNextFrequency();
  public:
  int direction;
  void emitSound(short *out );
  transmitContainer tContainer;
  SpectrumTransmitter ();
};

// Constructor
SpectrumTransmitter::SpectrumTransmitter()
{
  this->frequency = 1500;
  this->amplitudeMode = 0;
  this->direction = 1; // 1 is up, 0 is down
  buildWaveforms();
}

/*
  Emit Sound
  This function places the proper sinusoids in the output buffer
*/
void SpectrumTransmitter::emitSound( short *out )
{
  bool nextSinusoid = false;
  int phase = 0;

  for(int i=0; i < 1280; i++ )
  {
    if(nextSinusoid)
    {
      this->determineNextFrequency();
      nextSinusoid = false;
      phase = 0;
    }

    if(this->amplitudeMode)
    {
      *out++ = this->tContainer.waveforms[this->frequency + 10000].at(phase++);
    }else{
      *out++ = this->tContainer.waveforms[this->frequency].at(phase++);
    }
    if(phase >= this->tContainer.waveformSizes[this->frequency]) { nextSinusoid = true;}
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

void SpectrumTransmitter::buildWaveforms()
{
  int from_frequency = 1500;
  int to_frequency =   3000;
  int table_size;
  cout << "waveform sizes: ";
  for(int frequency = from_frequency; frequency <= to_frequency; frequency += 100)
  {
    table_size = 32000 / frequency;
    cout << table_size << ", ";
    this->tContainer.waveformSizes[frequency] = table_size;
  }
  cout << endl;

  map<int,int>::iterator it = this->tContainer.waveformSizes.begin();
  SAMPLE sample;

  // for each waveform size
  for(;it != this->tContainer.waveformSizes.end(); it++)
  {
    cout << "table " << it->first << ": ";
    for(int i=0; i< it->second; i++ )
    {
      // Full amplitude waveforms
      sample = (SAMPLE) (sin( ((double)i/(double) it->second) * M_PI * 2.0) * 32767);
      cout << sample << ", ";
      this->tContainer.waveforms[it->first].push_back(sample);

      // 70% amplitude waveforms.
      sample = (SAMPLE) (sin( ((double)i/(double) it->second) * M_PI * 2.0) * 32767 * 0.7);
      this->tContainer.waveforms[it->first + 10000].push_back(sample);
    }
    cout << endl;
  }
}

/*
  Determine Next frequency.

  modulation is achieved using the IncDec algorithm found in the
  Hermes paper.

  Once all the data is modulated, the function returns zeros and ones until the output
  buffer is full.

  TODO: Refactor
 */
void SpectrumTransmitter::determineNextFrequency()
{
  if(this->direction == 1)
  {
    if(this->frequency < 3000)
    {
      this->frequency+= 100;
    }else{
      this->direction = 0;
      this->frequency-= 100;
    }
  }else{
    if(this->frequency > 1500)
    {
      this->frequency -= 100;
    }else{
      this->direction = 1;
      this->frequency+= 100;
    }
  }
  // cout << frequency << endl;
  // if we are in the second half of a second, add 100 to the frequency
  // to adjust the amplitude.
  timeval time;
  gettimeofday(&time, NULL);
  long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
  this->amplitudeMode = (millis  % 1000) > 500;
};
