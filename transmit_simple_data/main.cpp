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

#define SAMPLE_RATE   (32000) // 8000 * 4
#define FRAMES_PER_BUFFER  (1280) // 320 * 4

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

using namespace std;


#define VLOW_TABLE_SIZE    (15)
#define LOW_TABLE_SIZE    (13)
#define MED_TABLE_SIZE    (11)
#define HIGH_TABLE_SIZE   (9)
#define HIGH (3)
#define MED  (2)
#define LOW  (1)
#define VLOW  (0)

typedef short SAMPLE;

typedef struct
{
  SAMPLE high_sine[HIGH_TABLE_SIZE];
  SAMPLE med_sine[MED_TABLE_SIZE];
  SAMPLE low_sine[LOW_TABLE_SIZE];
  SAMPLE vlow_sine[VLOW_TABLE_SIZE];
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

  bool nextSinusoid = true;
  int phase = 0;
  int mode;
  int message[8] = {0,1,2,3,2,3,2,3}; // produces 00010101 (repeats)
  int messagePosition = 0;
  for( int i=0; i<framesPerBuffer; i++ )
  {
    //--------------------------------------------------
    // this calculates the mode
    if(nextSinusoid)
    {
      nextSinusoid = false;
      phase = 0;
      mode = message[messagePosition++];
      // cout << mode;
      if(messagePosition > 7){ messagePosition = 0;}
    }
    //-----------------------------------------------
    // this outputs the sinusoid of the given mode
    if (mode == HIGH){
      *out++ = data->high_sine[phase];
      phase += 1;
      if(phase >= HIGH_TABLE_SIZE) { nextSinusoid = true;}
    }else if(mode == MED){
      *out++ = data->med_sine[phase];
      phase += 1;
      if(phase >= MED_TABLE_SIZE) { nextSinusoid = true;}
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
  for( i=0; i<HIGH_TABLE_SIZE; i++ )
  {
    data.high_sine[i] = (SAMPLE) (sin( ((double)i/(double)HIGH_TABLE_SIZE) * M_PI * 2.0) * 32767);
  }

  for( i=0; i<MED_TABLE_SIZE; i++ )
  {
    data.med_sine[i] = (SAMPLE) (sin( ((double)i/(double)MED_TABLE_SIZE) * M_PI * 2.0) * 32767);
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

  outputParameters.device =2; // Pa_GetDefaultOutputDevice(); /* default output device */
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


