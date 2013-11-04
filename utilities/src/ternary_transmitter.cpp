#include <iostream>
#include "../include/constants.h"
#include "../include/ternary_transmitter.h"

/*
  The Transmitter's job is to produce audio that encodes
  the data.
 */

/*
 The Constructor is expensive, as it builds waveforms that
 correspond to each of the five frequencies used by the
 Tiptoe protocol.
 */

Transmitter::Transmitter (int framesPerBuffer, int low_freq, int high_freq)
{
  this->framesPerBuffer = framesPerBuffer;
  this->tContainer.waveformSizes[PREAMBLE_LOW] = (double) SAMPLE_RATE / (double) PREAMBLE_LOW_FREQUENCY;
  this->tContainer.waveformSizes[EDGE_LOW] = (double) SAMPLE_RATE / (double) EDGE_LOW_FREQUENCY;
  this->tContainer.waveformSizes[MIDDLE_LOW] = (double) SAMPLE_RATE / (double) MIDDLE_LOW_FREQUENCY;
  this->tContainer.waveformSizes[MIDDLE_HIGH] = (double) SAMPLE_RATE / (double) MIDDLE_HIGH_FREQUENCY;
  this->tContainer.waveformSizes[EDGE_HIGH] = (double) SAMPLE_RATE / (double) EDGE_HIGH_FREQUENCY;
  this->tContainer.waveformSizes[TEST_HIGH] = (double) SAMPLE_RATE / (double) TEST_HIGH_FREQUENCY;
  buildWaveforms();

  low_frequency = low_freq;
  high_frequency = high_freq;
}

/*
 Resets the transmitter with the new data to send
*/


/*
  Emit Sound
  This function places the proper sinusoids in the output buffer

  The function's single argument is a pointer to the Portaudio
  ouput buffer.
*/
void Transmitter::emitSound( short *out )
{
  position = 0;
  bool nextSinusoid = false;
  int phase = 0;
  int mode = low_frequency;
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

    // Here we play with amplitude, shaping it to look like spoken
    // words in order to trick the telco codecs.
    // amp = 0.5;
    // if( i <= (fpb / 2))
    // {
      // amp += i/fpb;
    // }else{
      // amp += (fpb - i) / fpb;
    // }
    amp = 1;
    if(this->amplitudeMode)
    {
      amp = amp * this->tContainer.waveforms[mode + 100].at(phase++);
    }else{
      amp = amp * this->tContainer.waveforms[mode].at(phase++);
    }
    // output the same thing on both channels
    *out++ = amp;
    *out++ = amp;
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
   See http://en.wikipedia.org/wiki/Voice_activity_detection
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

  TODO: Refactor this huge function
 */
int Transmitter::determineNextMode(int mode, int onFrame)
{
  int pattern[54] = { 4,4,4,3,3,3,5,5,5,4,4,4,3,3,3,5,5,5,
                      4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
                      5,5,5,4,4,4,5,5,5,4,4,4,5,5,5,4,4,4 };

  position = (position + 1) % 54;
  mode = pattern[position];

  // if we are in the second half of a second, add 100 to the mode
  // to adjust the amplitude.
  timeval time;
  gettimeofday(&time, NULL);
  long millis = time.tv_usec / 1000;
  millis = millis % 1000;
  amplitudeMode = (millis < 250) || (millis > 500 && millis < 750);
  return mode;
};
