/*
 * Transmit is the lowest level in the stack,
 * taking a raw bit stream and converting to
 * analog using the IncDec algorithm
 */

#include <stdio.h>
#include <iostream>
#include <csignal>
#include <fstream>
#include <vector>
#include <bitset>
#include <math.h>
#include "portaudio.h"
#include "transcode.cpp"

#define SAMPLE_RATE   (6000)
#define FRAMES_PER_BUFFER  (320)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

using namespace std;

#define LOW_TABLE_SIZE    (4) // 1.5kHz
#define BASE_TABLE_SIZE   (3) // 3kHz
#define HIGH_TABLE_SIZE   (2) // 4kHz
#define ZERO (0)
#define LOW  (1)
#define BASE (2)
#define HIGH (3)

typedef short SAMPLE;

typedef struct
{
  SAMPLE base_sine[BASE_TABLE_SIZE];
  SAMPLE high_sine[HIGH_TABLE_SIZE];
  SAMPLE low_sine[LOW_TABLE_SIZE];
}
transmitData;

static int transmitCallback( const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData )
{
  transmitData *data = (transmitData*)userData;
  SAMPLE *out = (SAMPLE*)outputBuffer;

  (void) timeInfo; /* Prevent unused variable warnings. */
  (void) statusFlags;
  (void) inputBuffer;

  // faux input -- some random bytes
  vector<unsigned char> faux;

  // 48 bits will come from Codec2...
  faux.push_back ( 0xD3 );
  faux.push_back ( 0xA9 );
  faux.push_back ( 0x5D );
  faux.push_back ( 0x2D );
  faux.push_back ( 0xBC );
  faux.push_back ( 0x94 );


  vector<bool> transcodedBits;
  Transcode::Perform(faux, transcodedBits);
  vector<bool>::iterator bitIterator = transcodedBits.begin();
  bitIterator = transcodedBits.begin();
  bool nextSinusoid = true;
  int phase = 0;

  printf("transcoded bit count: %d\n", (int) transcodedBits.size());
  int mode = BASE;
  int totalFrames = 0;
  for(;bitIterator != transcodedBits.end(); bitIterator++)
  {
    if(*bitIterator == 1)
    {
      mode += 1;
    }else{
      mode -= 1;
    }
    //--------------------------------
    if(mode == 0)
    {
      totalFrames += LOW_TABLE_SIZE;
    }else if(mode == 1)
    {
      totalFrames += BASE_TABLE_SIZE;
    }else{
      totalFrames += HIGH_TABLE_SIZE;
    }
    //--------------------------------
    if(mode > HIGH || mode < LOW)
    {
      cout << endl << "mode is out of range.  It is now: " << mode << endl;
      return 1;
    }
  }
  printf("totalFrames: %d\n", totalFrames);
  int neededFrames =  FRAMES_PER_BUFFER - totalFrames;
  printf("neededFrames: %d\n", neededFrames);

  short fillFrames[FRAMES_PER_BUFFER];

  for( i=0; i< neededFrames; i++ )
  {
    fillFrames[i] = (SAMPLE) (cos( ((double)i/(double)neededFrames) * M_PI * 2.0) * 32767);
  }

  bool filledYet = false;

  bitIterator = transcodedBits.begin();
  mode = BASE; // reset

  for( int i=0; i<framesPerBuffer; i++ )
  {
    //--------------------------------------------------
    // this calculates the mode
    if(nextSinusoid)
    {
      nextSinusoid = false;
      phase = 0;
      if(bitIterator > transcodedBits.end())
      {
        mode = ZERO;
      }else{
        if (bitIterator == transcodedBits.begin())
        {
          mode = BASE;
        }
        if(*bitIterator == 1)
        {
          mode += 1;
        }else{
          mode -= 1;
        }
        if(mode == LOW && filledYet == false)
        {
          filledYet = true;
          // replace the low table with the super low table
        }
        if(mode > 5 || mode < ZERO)
        {
          cout << endl << "data-> mode is out of range.  It is now: " << mode << endl;
          cout << "occurred at: " << i << endl;
          return 1;
        }
        bitIterator += 1;
      }
    }
    //-----------------------------------------------
    // this outputs the sinusoid of the given mode
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
    }else{
      *out++ = 0;
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
    data.base_sine[i] = (SAMPLE) (cos( ((double)i/(double)BASE_TABLE_SIZE) * M_PI * 2.0) * 32767);
  }

  for( i=0; i<HIGH_TABLE_SIZE; i++ )
  {
    data.high_sine[i] = (SAMPLE) (cos( ((double)i/(double)HIGH_TABLE_SIZE) * M_PI * 2.0) * 32767);
  }

  for( i=0; i<LOW_TABLE_SIZE; i++ )
  {
    data.low_sine[i] = (SAMPLE) (cos( ((double)i/(double)LOW_TABLE_SIZE) * M_PI * 2.0) * 32767);
  }

  err = Pa_Initialize();
  if( err != paNoError ) goto error;

  outputParameters.device =3; // Pa_GetDefaultOutputDevice(); /* default output device */
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


