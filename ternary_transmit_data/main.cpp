#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <csignal>
#include <fstream>
#include <stdlib.h>
#include "portaudio.h"
#include "../utilities/include/constants.h"
#include "../utilities/include/converter.h"
#include "../utilities/include/ternary_transmitter.h"

using namespace std;

typedef short SAMPLE;

/*----------------------------------------------------------------
 * Callback Data.  Persists across buffers.  Used here to hold on
 * to large objects that take time to instantiate.  The Transmitter
 * stores the waveforms, for example.
 */
typedef struct
{
  // Points to the instance of codec2
  // CODEC2                  *codec2;

  // Points to the instance of the transmitter, which holds
  // waveforms that are expensive to compute, so we do this
  // only once.
  Transmitter             *transmitter;

  // Used to ensure both sides of the conversation are in
  // sync.
  int                     bufferCounter;
}
callbackData;

static int transmitCallback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData )
{
  callbackData *data = (callbackData*)userData;
  SAMPLE *out = (SAMPLE*)outputBuffer;

  (void) timeInfo; /* Prevent unused variable warnings. */
  (void) statusFlags;
  (void) inputBuffer;

  data->transmitter->emitSound(out);
  return paContinue;
}

/*******************************************************************/
int main(int argc, char *argv[])
{
  int device_id;
  if(argc != 2)
  {
    cout << "Usage: " <<argv[0] << " <device id (int)>" << endl;
    cout << "try running port-audio's pa_devs binary" << endl;
    cout << "or if you are on a Raspberry Pi, try $aplay -l" << endl;
    exit(1);
  }else{
    device_id = atoi(argv[1]);
  }
  PaStreamParameters outputParameters;
  PaStream *stream;
  PaError err;
  callbackData data;

  Transmitter transmitter(FRAMES_PER_BUFFER);
  data.transmitter = &transmitter;

  data.bufferCounter = 0;

  err = Pa_Initialize();
  if( err != paNoError ) goto error;

  outputParameters.device = device_id;// Pa_GetDefaultOutputDevice(); /* default output device */
  if (outputParameters.device == paNoDevice) {
    fprintf(stderr,"Error: No default output device.\n");
    goto error;
  }
  outputParameters.channelCount = 2;       /* stereo output */
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
  printf("Finished.\n");

  return err;
error:
  Pa_Terminate();
  fprintf( stderr, "An error occured while using the portaudio stream\n" );
  fprintf( stderr, "Error number: %d\n", err );
  return err;
}


