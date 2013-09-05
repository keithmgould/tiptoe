#include <stdio.h>
#include <iostream>
#include <csignal>
#include <fstream>
#include "portaudio.h"
#include "../utilities/transcode.cpp"
#include "../utilities/transmit.cpp"

#define SAMPLE_RATE   (32000) // 8000 * 4
#define FRAMES_PER_BUFFER  (1280) // 320 * 4

using namespace std;

typedef short SAMPLE;

static int transmitCallback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData )
{
  Transmitter *transmitter = (Transmitter*)userData;
  SAMPLE *out = (SAMPLE*)outputBuffer;

  (void) timeInfo; /* Prevent unused variable warnings. */
  (void) statusFlags;
  (void) inputBuffer;

  // 6 bytes (48 bits) will come from Codec2 so faux input: 6 random bytes
  // which when transcoded becomes:
  // 101001100101101010011001100101100110011010100110010110011010011010011010101001011001011001100101
  unsigned char faux[6] = { 0xD3, 0xA9, 0x5D, 0x2D, 0xBC, 0x94 };
  vector<bool> transcodedBits;
  Transcode::Perform(faux, transcodedBits, 6);
  transmitter->setBits(transcodedBits);
  transmitter->emitSound(out);
  return paContinue;
}

/*******************************************************************/
int main(void)
{
  PaStreamParameters outputParameters;
  PaStream *stream;
  PaError err;
  Transmitter transmitter(FRAMES_PER_BUFFER);

  err = Pa_Initialize();
  if( err != paNoError ) goto error;

  outputParameters.device = 3; // Pa_GetDefaultOutputDevice(); /* default output device */
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


