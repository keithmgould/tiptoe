#include <stdio.h>
#include <iostream>
#include <csignal>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <map>
#include "portaudio.h"
#include "../utilities/transmit_spectrum.cpp"

#define SAMPLE_RATE   (32000) // 8000 * 4
#define FRAMES_PER_BUFFER  (1280) // 320 * 4

using namespace std;

typedef short SAMPLE;

static int transmitCallback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData )
{
  SpectrumTransmitter *transmitter = (SpectrumTransmitter*)userData;
  SAMPLE *out = (SAMPLE*)outputBuffer;

  (void) timeInfo; /* Prevent unused variable warnings. */
  (void) statusFlags;
  (void) inputBuffer;

  transmitter->emitSound(out);
  return paContinue;
}

/*******************************************************************/
int main(void)
{
  PaStreamParameters outputParameters;
  PaStream *stream;
  PaError err;
  SpectrumTransmitter transmitter;

  err = Pa_Initialize();
  if( err != paNoError ) goto error;

  outputParameters.device = 4; // Pa_GetDefaultOutputDevice(); /* default output device */
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
      &transmitter );
  if( err != paNoError ) goto error;

  err = Pa_StartStream( stream );
  if( err != paNoError ) goto error;

  cout << "Hit ENTER to stop program." << endl;
  getchar();
  err = Pa_CloseStream( stream );
  if( err != paNoError ) goto error;

  Pa_Terminate();
  printf("Finished.\n");

  return err;
error:
  Pa_Terminate();
  fprintf( stderr, "An error occured while using the portaudio stream\n" );
  fprintf( stderr, "Error number: %d\n", err );
  return err;
}


