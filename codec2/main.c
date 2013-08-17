#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "codec2.h"
#include "portaudio.h"


/*----------------------------------------------------------------
 * Since we are compressing and working with voice, we can go as
 * low as possible.  6kHz might work but for now lets stick with
 * 8kHz.
 */
#define SAMPLE_RATE         (8000)

/*----------------------------------------------------------------
 * The FRAMES_PER_BUFFER and CODEC2_MODE constants are correlated.
 * See codec2.c in the codec2 repo to learn more.  In short, do not
 * change one without learning how to change the other.
*/
#define CODEC2_MODE         (CODEC2_MODE_1200)
#define FRAMES_PER_BUFFER   (320)

/*----------------------------------------------------------------
 * The port audio sample type and SAMPLE typedef are correlated.
 * If we were to use paFloat32 for example, the SAMPLE type would
 * be float.
 */
#define PA_SAMPLE_TYPE      paInt16
typedef short SAMPLE;


/*----------------------------------------------------------------
 * callbackData contains any information needed to persist between
 * callbacks. We use it so that we don't have to initialize codec2
 * or its arrays repeatedly.
 */
typedef struct
{
  void          *codec2;
  int           nsam;       // number of samples
  int           nbit;       // number of bits
  unsigned char *bits;
}
callbackData;


/*----------------------------------------------------------------
 * localToRemoteCallback is called everytime the inputBuffer is full.
 *
 * local audio comes directly in from the user.
 * remote audio comes in over the telco's line (the phone line)
 *
 * localToRemoteCallback's job is to take local audio and:
 * 1) compress.  Using codec2
 * 2) encrypt.  Using AES256
 * 3) modulate.  Using IncDec (see "Hermes: Data Transmission over Unknown Voice Channels")
 */
static int localToRemoteCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData )
{
  callbackData *data = (callbackData*)userData;
  SAMPLE *out = (SAMPLE*)outputBuffer;
  const SAMPLE *in = (const SAMPLE*)inputBuffer;
  int i;
  (void) timeInfo; /* Prevent unused variable warnings. */
  (void) statusFlags;
  (void) userData;

  short monoBuffer[FRAMES_PER_BUFFER];

  if( inputBuffer == NULL )
  {
    for( i=0; i<framesPerBuffer; i++ )
    {
      *out++ = 0;
      *out++ = 0;
    }
  }
  else
  {
    for( i=0; i<framesPerBuffer; i++ )
    {
      monoBuffer[i] = *in++;
      *in++;
    }
    codec2_encode(data->codec2, data->bits, monoBuffer);
    codec2_decode(data->codec2, monoBuffer, data->bits);

    for( i=0; i<framesPerBuffer; i++ )
    {
      *out++ = monoBuffer[i] * 2;
      *out++ = monoBuffer[i] * 2;
    }
  }

  return paContinue;
}

/*******************************************************************/
int main(void)
{
  PaStreamParameters inputParameters, outputParameters;
  PaStream *stream;
  PaError err;
  callbackData data;

  err = Pa_Initialize();
  if( err != paNoError ) goto error;

  /* create a pointer to the codec states */
  data.codec2 = codec2_create(CODEC2_MODE);

  /* determine the number of bits per frame */
  data.nbit = codec2_bits_per_frame(data.codec2);

  /* allocate and clean space needed to store the compressed audio */
  data.bits = malloc (data.nbit * sizeof(char) );
  int i;
  for (i = 0; i < data.nbit; i++) { data.bits[i] = 0; }

  inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
  if (inputParameters.device == paNoDevice) {
    fprintf(stderr,"Error: No default input device.\n");
    goto error;
  }
  inputParameters.channelCount = 2;
  inputParameters.sampleFormat = PA_SAMPLE_TYPE;
  inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
  inputParameters.hostApiSpecificStreamInfo = NULL;

  outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
  if (outputParameters.device == paNoDevice) {
    fprintf(stderr,"Error: No default output device.\n");
    goto error;
  }
  outputParameters.channelCount = 2;
  outputParameters.sampleFormat = PA_SAMPLE_TYPE;
  outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(
      &stream,
      &inputParameters,
      &outputParameters,
      SAMPLE_RATE,
      FRAMES_PER_BUFFER,
      0, /* paClipOff, */  /* we won't output out of range samples so don't bother clipping them */
      localToRemoteCallback,
      &data );
  if( err != paNoError ) goto error;

  err = Pa_StartStream( stream );
  if( err != paNoError ) goto error;

  printf("Hit ENTER to stop program.\n");
  getchar();
  err = Pa_CloseStream( stream );
  if( err != paNoError ) goto error;

  printf("Finished.\n");
  Pa_Terminate();
  free(data.bits);
  codec2_destroy(data.codec2);
  return 0;

error:
  Pa_Terminate();
  free(data.bits);
  codec2_destroy(data.codec2);
  fprintf( stderr, "An error occured while using the portaudio stream\n" );
  fprintf( stderr, "Error number: %d\n", err );
  fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
  return -1;
}
