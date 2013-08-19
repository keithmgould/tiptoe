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

#define VSLOW_TABLE_SIZE    (6) // 1.0kHz
#define SLOW_TABLE_SIZE    (5) // 1.2kHz
#define LOW_TABLE_SIZE    (4) // 1.5kHz
#define BASE_TABLE_SIZE   (3) // 3kHz
#define HIGH_TABLE_SIZE   (2) // 4kHz
#define ZERO  (0)
#define HIGH  (1)
#define BASE  (2)
#define LOW   (3)
#define SLOW  (4)
#define VSLOW (5)

typedef short SAMPLE;

typedef struct
{
  SAMPLE base_sine[BASE_TABLE_SIZE];
  SAMPLE high_sine[HIGH_TABLE_SIZE];
  SAMPLE low_sine[LOW_TABLE_SIZE];
  SAMPLE slow_sine[SLOW_TABLE_SIZE];
  SAMPLE vslow_sine[VSLOW_TABLE_SIZE];
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

  int mode = BASE;
  int totalFrames = 0;
  int lowCount = 0;
  int highCount = 0;
  int baseCount = 0;
  for(;bitIterator != transcodedBits.end(); bitIterator++)
  {
    if(*bitIterator == 1)
    {
      mode += 1;
    }else{
      mode -= 1;
    }
    //--------------------------------
    if(mode == LOW)
    {
      lowCount += 1;
      totalFrames += LOW_TABLE_SIZE;
    }else if(mode == BASE)
    {
      baseCount += 1;
      totalFrames += BASE_TABLE_SIZE;
    }else if(mode == HIGH){
      highCount += 1;
      totalFrames += HIGH_TABLE_SIZE;
    }else{
      cout << endl << "mode is out of range.  It is now: " << mode << endl;
      return 1;
    }
  }
  // printf("transcoded bit count: %d\n", (int) transcodedBits.size());
  // printf("totalFrames: %d\n", totalFrames);
  // printf("lowCount: %d\n", lowCount);
  // printf("baseCount: %d\n", baseCount);
  // printf("highCount: %d\n", highCount);
  int neededFrames =  FRAMES_PER_BUFFER - totalFrames;
  // printf("neededFrames: %d\n", neededFrames);

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
        mode = ZERO; // this should never happen
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
        // replace the low table with the super low table
        if(mode == LOW)
        {
          if(neededFrames >= 2)
          {
            neededFrames -= 2;
            mode = VSLOW;
          }else if(neededFrames == 1){
            neededFrames -= 1;
            mode = SLOW;
          }
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
    }else if(mode == SLOW){
      *out++ = data->slow_sine[phase];
      phase += 1;
      if(phase >= SLOW_TABLE_SIZE)
      {
        nextSinusoid = true;
        mode = LOW;
      }
    }else if(mode == VSLOW){
      *out++ = data->vslow_sine[phase];
      phase += 1;
      if(phase >= VSLOW_TABLE_SIZE)
      {
        nextSinusoid = true;
        mode = LOW;
      }

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

  for( i=0; i<SLOW_TABLE_SIZE; i++ )
  {
    data.slow_sine[i] = (SAMPLE) (cos( ((double)i/(double)LOW_TABLE_SIZE) * M_PI * 2.0) * 32767);
  }

  for( i=0; i<VSLOW_TABLE_SIZE; i++ )
  {
    data.vslow_sine[i] = (SAMPLE) (cos( ((double)i/(double)LOW_TABLE_SIZE) * M_PI * 2.0) * 32767);
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


