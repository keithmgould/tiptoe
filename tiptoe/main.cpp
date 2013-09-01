#include <stdio.h>
#include <stdlib.h>
#include "codec2.h"
#include "portaudio.h"
#include "../utilities/transcode.cpp"
#include "../utilities/downsample.cpp"



/*----------------------------------------------------------------
 * To ensure we "shape" each sinusoid with enough resolution,
 * the output sample rate must be high enough that we can
 * distinguish between frequencies such as 2.5kHz and 2.7kHz.
 * Since we shape our sinusoids by stating the amount of frames
 * a single sinusoid must extend over, we can only work with
 * integers to define the number of frames.  This leads us
 * to a somewhat high sample rate of 32kHz, which is used for
 * both input and output.
 *
 * Note however that when we feed the audo to Codec2, we first
 * downstample to 8kHz.
 */
#define SAMPLE_RATE         (32000)

/*----------------------------------------------------------------
 * We are pressed for bandwidth so for the time being we use the
 * most compression Codec2 has to offer, which is 1200 bps.  Note
 * that in this mode, Codec2 uses an input of 320 frames, which
 * at 8kHz is 40ms.  Since we are not sampling at 8kHz, but at
 * 32kHz, we increase our buffer here accordingly.  This way when
 * we downsample, (by keeping 1/4 frames) we will end up with 320
 * frames which we can pass on to Codec2.
*/
#define CODEC2_MODE         (CODEC2_MODE_1200)
#define FRAMES_PER_BUFFER   (1280)

/*----------------------------------------------------------------
 * The port audio sample type and SAMPLE typedef are correlated.
 * If we were to use paFloat32 for example, the SAMPLE type would
 * be float.
 *
 * 2 bytes (16 bit ints) work just fine for voice.
 */
#define PA_SAMPLE_TYPE      paInt16
typedef short SAMPLE;


/*----------------------------------------------------------------
 * callbackData contains any information needed to persist between
 * callbacks (across buffers). We use it so that we don't have to
 * initialize codec2 or its arrays repeatedly.
 */
typedef struct
{
  CODEC2        *codec2;
}
callbackData;

/*----------------------------------------------------------------
 * localToRemoteCallback is called everytime the buffer is full.
 *
 * local audio comes directly in from the user.
 * remote audio comes in over the telco's line (the phone line)
 *
 * localToRemoteCallback's job is to take local audio and:
 * 1) compress.  Using codec2
 * 2) encrypt.  Using AES256
 * 3) modulate.  Using IncDec (see "Hermes: Data Transmission over Unknown Voice Channels")
 */

static int localToRemoteCallback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData )
{
  callbackData *data = (callbackData*)userData;
  SAMPLE *out = (SAMPLE*)outputBuffer;
  SAMPLE *in = (SAMPLE*)inputBuffer;
  (void) timeInfo; /* Prevent unused variable warnings. */
  (void) statusFlags;
  (void) userData;
  SAMPLE downsampled[320];
  unsigned char compressed[6];
  bool transcodedBits[96];

  if( inputBuffer == NULL )
  {
    for( int i=0; i<framesPerBuffer; i++ )
    {
      *out++ = 0;
    }
  }
  else
  {
    // downsample 1280 samples to 320 samples
    // by taking every 4th sample
    Downsample::Perform(in, downsampled, 1280, 4);

    // compress 320 samples to 48 bits (6 bytes)
    codec2_encode(data->codec2, compressed, downsampled);

    // transcode
    Transcode::Perform(compressed, transcodedBits, 6);
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
  int nbit;
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
  inputParameters.sampleFormat = PA_SAMPLE_TYPE;
  inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
  inputParameters.hostApiSpecificStreamInfo = NULL;

  outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
  if (outputParameters.device == paNoDevice) {
    fprintf(stderr,"Error: No default output device.\n");
    goto error;
  }
  outputParameters.channelCount = 1;
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
