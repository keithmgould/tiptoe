/*
 * Transmit is the lowest level in the stack,
 * taking a raw bit stream and converting to
 * analog using the IncDec algorithm
 */

#include <stdio.h>
#include <iostream>
#include <csignal>
#include <fstream>
#include <math.h>
#include "portaudio.h"
#include "../utilities/transcode.cpp"

#define SAMPLE_RATE   (32000) // 8000 * 4
#define FRAMES_PER_BUFFER  (1280) // 320 * 4

#ifndef M_PI
#define M_PI  (3.14159265) // as determined with a nail, twine and chalk.
#endif

using namespace std;

#define VLOW_TABLE_SIZE   (14)
#define LOW_TABLE_SIZE    (13)
#define BASE_TABLE_SIZE   (12)
#define HIGH_TABLE_SIZE   (11)
#define VLOW  (1)
#define LOW   (2)
#define BASE  (3)
#define HIGH  (4)

typedef short SAMPLE;

typedef struct
{
  SAMPLE base_sine[BASE_TABLE_SIZE];
  SAMPLE high_sine[HIGH_TABLE_SIZE];
  SAMPLE low_sine[LOW_TABLE_SIZE];
  SAMPLE vlow_sine[VLOW_TABLE_SIZE];
}
transmitData;

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

/*
  Determine Next Node.
  Each buffer begins with preamble consisting of 4 sinusoids,
  one of which is not possible to use during normal
  modulation (this is the VLOW frequency).  The VLOW sinusoid means that the preamble
  emits three consecutive zeros, which is not possible during norming modulation

  After the preamble, modulation is achieved using the IncDec algorithm found in the
  Hermes paper: a 1 yields a lower frequency, and a 0 yields a higher frequency.
 */
void determineNextMode(int *mode, int *preamble, bool transcodedBits[], int *bitIterator)
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

static int transmitCallback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData )
{
  transmitData *data = (transmitData*)userData;
  SAMPLE *out = (SAMPLE*)outputBuffer;

  (void) timeInfo; /* Prevent unused variable warnings. */
  (void) statusFlags;
  (void) inputBuffer;

  // faux input: 6 random bytes
  // 48 bits will come from Codec2...
  // which when transcoded:
  // 101001100101101010011001100101100110011010100110010110011010011010011010101001011001011001100101
  unsigned char faux[6] = { 0xD3, 0xA9, 0x5D, 0x2D, 0xBC, 0x94 };
  bool transcodedBits[96];
  Transcode::Perform(faux, transcodedBits, 6);


  int i;
  bool nextSinusoid = true;
  int preamble = 0;
  int phase = 0;
  int mode = BASE;
  int bitIterator = 0;

  //-----------------------------------------
  // cout << endl << endl;
  // for(i=0;i<96;i++)
  // {
    // cout << transcodedBits[i];
  // }
  // cout << endl << endl;
  //-----------------------------------------

  for(i=0; i<framesPerBuffer; i++ )
  {
    if(nextSinusoid)
    {
      determineNextMode(&mode, &preamble, transcodedBits, &bitIterator);
      nextSinusoid = false;
      phase = 0;
    }
    if (mode == HIGH){
      *out++ = data->high_sine[phase];
      phase += 1;
      if(phase >= HIGH_TABLE_SIZE) { nextSinusoid = true;}
    }else if(mode == BASE){
      *out++ = data->base_sine[phase];
      phase += 1;
      if(phase >= BASE_TABLE_SIZE) { nextSinusoid = true;}
    }else if(mode == LOW){
      *out++ = data->low_sine[phase];
      phase += 1;
      if(phase >= LOW_TABLE_SIZE) { nextSinusoid = true;}
    }else if(mode == VLOW){
      *out++ = data->vlow_sine[phase];
      phase += 1;
      if(phase >= VLOW_TABLE_SIZE) { nextSinusoid = true;}
    }else{
      printf("we got to a bad place sir.\n");
      return 1;
    }
  }
  return paContinue;
}

/*******************************************************************/
int main(void)
{
  PaStreamParameters outputParameters;
  PaStream *stream;
  PaError err;
  transmitData data;
  int i;

  /* initialise sinusoidal wavetables */
  for( i=0; i<BASE_TABLE_SIZE; i++ )
  {
    data.base_sine[i] = (SAMPLE) (sin( ((double)i/(double)BASE_TABLE_SIZE) * M_PI * 2.0) * 32767);
  }

  for( i=0; i<HIGH_TABLE_SIZE; i++ )
  {
    data.high_sine[i] = (SAMPLE) (sin( ((double)i/(double)HIGH_TABLE_SIZE) * M_PI * 2.0) * 32767);
  }

  for( i=0; i<LOW_TABLE_SIZE; i++ )
  {
    data.low_sine[i] = (SAMPLE) (sin( ((double)i/(double)LOW_TABLE_SIZE) * M_PI * 2.0) * 32767);
  }
  for( i=0; i<VLOW_TABLE_SIZE; i++ )
  {
    data.vlow_sine[i] = (SAMPLE) (sin( ((double)i/(double)VLOW_TABLE_SIZE) * M_PI * 2.0) * 32767);
  }

  err = Pa_Initialize();
  if( err != paNoError ) goto error;

  outputParameters.device = 2; // Pa_GetDefaultOutputDevice(); /* default output device */
  if (outputParameters.device == paNoDevice) {
    fprintf(stderr,"Error: No default output device.\n");
    goto error;
  }
  outputParameters.channelCount = 1;       /* mono output */
  outputParameters.sampleFormat = paInt16;
  outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(
      &stream,
      NULL, /* no input */
      &outputParameters,
      SAMPLE_RATE,
      FRAMES_PER_BUFFER,
      paClipOff,
      transmitCallback,
      &data );
  if( err != paNoError ) goto error;

  err = Pa_StartStream( stream );
  if( err != paNoError ) goto error;

  cout << "Hit ENTER to stop program." << endl;
  getchar();
  err = Pa_CloseStream( stream );
  if( err != paNoError ) goto error;

  Pa_Terminate();
  printf("Test finished.\n");

  return err;
error:
  Pa_Terminate();
  fprintf( stderr, "An error occured while using the portaudio stream\n" );
  fprintf( stderr, "Error number: %d\n", err );
  return err;
}


