#include <vector>
#include <samplerate.h>
#include "codec2.h"
#include "portaudio.h"
#include "../utilities/constants.cpp"
#include "../utilities/downsample.cpp"
#include "../utilities/upsample.cpp"

/*----------------------------------------------------------------
 * Since we are compressing and working with voice, we can go as
 * low as possible.  6kHz might work but for now lets stick with
 * 8kHz.
 */
#define short_RATE         (32000)

/*----------------------------------------------------------------
 * The FRAMES_PER_BUFFER and CODEC2_MODE constants are correlated.
 * See codec2.c in the codec2 repo to learn more.  In short, do not
 * change one without learning how to change the other.
 * mode_1200: 320
 * mode_1400: 320
 * mode_2400: 160
 * mode_3200: 160
*/
#define CODEC2_MODE         (CODEC2_MODE_1200)
#define FRAMES_PER_BUFFER   (1280)

/*----------------------------------------------------------------
 * The port audio sample type and short typedef are correlated.
 * If we were to use paFloat32 for example, the short type would
 * be float.
 */
#define PA_short_TYPE      paInt16


/*----------------------------------------------------------------
 * callbackData contains any information needed to persist between
 * callbacks. We use it so that we don't have to initialize codec2
 * or its arrays repeatedly.
 */
typedef struct
{
  CODEC2        *codec2;
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
  short *out = (short*)outputBuffer;
  short *in = (short*)inputBuffer;
  (void) timeInfo; /* Prevent unused variable warnings. */
  (void) statusFlags;
  (void) userData;
  unsigned char compressed[6];
  short downsampled[320];
  short upsampled[1280];
  short postCompressed[320];
  if( inputBuffer == NULL )
  {
    for(int i=0; i<framesPerBuffer; i++ )
    {
      *out++ = 0;
    }
  }
  else
  {
    // downsample 1280 samples to 320 samples by taking every 4th sample
    Downsample::Perform(in, downsampled, 1280, 4);

    // upsample 320 samples to 1280 samples by interpolating
    Upsample::Perform(downsampled,out, 320);
  }

  return paContinue;
}

/*******************************************************************/
int main(void)
{
  int src_error;

  PaStreamParameters inputParameters, outputParameters;
  PaStream *stream;
  PaError err;
  callbackData data;

  err = Pa_Initialize();
  if( err != paNoError ) goto error;

  /* create a pointer to the codec states */
  data.codec2 = codec2_create(CODEC2_MODE);

  inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
  if (inputParameters.device == paNoDevice) {
    fprintf(stderr,"Error: No default input device.\n");
    goto error;
  }
  inputParameters.channelCount = 1;
  inputParameters.sampleFormat = PA_short_TYPE;
  inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
  inputParameters.hostApiSpecificStreamInfo = NULL;

  outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
  if (outputParameters.device == paNoDevice) {
    fprintf(stderr,"Error: No default output device.\n");
    goto error;
  }
  outputParameters.channelCount = 1;
  outputParameters.sampleFormat = PA_short_TYPE;
  outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(
      &stream,
      &inputParameters,
      &outputParameters,
      short_RATE,
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
  codec2_destroy(data.codec2);
  return 0;

error:
  Pa_Terminate();
  codec2_destroy(data.codec2);
  fprintf( stderr, "An error occured while using the portaudio stream\n" );
  fprintf( stderr, "Error number: %d\n", err );
  fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
  return -1;
}
