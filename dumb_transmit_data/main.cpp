#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <csignal>
#include <fstream>
#include <stdlib.h>
#include "portaudio.h"
#include "../utilities/include/constants.h"
#include "../utilities/include/converter.h"
#include "../utilities/include/dumb_transmitter.h"

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

  // 6 bytes (48 bits) will come from Codec2 so faux input: 6 random bytes
  unsigned char faux[6] = { 0xD3, 0xA9, 0x5D, 0x2D, 0xBC, 0x94 }; //110100111010100101011101001011011011110010010100
  // unsigned char faux[6] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, };   //101010101010101010101010101010101010101010101010
  // convert and push unsigned chars onto dataBits
  vector<bool> dataBits;
  vector<bool> transcodedBits;
  Converter::UnsignedCharToBits(faux, dataBits, 6);
  data->transmitter->emitSound(out);
  return paContinue;
}

/*******************************************************************/
int main(int argc, char *argv[])
{
  int device_id;
  int low_freq;
  int high_freq;
  if(argc != 4)
  {
    cout << "Usage: " <<argv[0] << " <device id (int)> <low freq mode(int)> <high freq mode(int)>" << endl;
    cout << "try running port-audio's pa_devs binary" << endl;
    cout << "or if you are on a Raspberry Pi, try $aplay -l" << endl;
    cout << "check utilities/include/constants.h for frequency modes" << endl;
    cout << "example: ./main 4 3 5" << endl;
    exit(1);
  }else{
    device_id = atoi(argv[1]);
    low_freq = atoi(argv[2]);
    high_freq = atoi(argv[3]);
  }
  PaStreamParameters outputParameters;
  PaStream *stream;
  PaError err;
  callbackData data;

  Transmitter transmitter(FRAMES_PER_BUFFER, low_freq, high_freq);
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


