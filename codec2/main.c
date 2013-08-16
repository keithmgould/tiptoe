#include "codec2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "portaudio.h"
/*
** Note that many of the older ISA sound cards on PCs do NOT support
** full duplex audio (simultaneous record and playback).
** And some only support full duplex at lower sample rates.
*/
#define SAMPLE_RATE         (8000)
#define PA_SAMPLE_TYPE      paInt16
#define FRAMES_PER_BUFFER   (320)

typedef short SAMPLE;

typedef struct
{
  void          *codec2;
  int           nsam; // number of samples
  int           nbit; // number of bits
  int           nbyte;// number of bytes
  short         *buf;
  unsigned char *bits;
}
paTestData;

static int fuzzCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData )
{
    paTestData *data = (paTestData*)userData;
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
            *out++ = 0;
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
    paTestData data;

    err = Pa_Initialize();
    if( err != paNoError ) goto error;


    /* create a pointer to the codec states */
    data.codec2 = codec2_create(CODEC2_MODE_1200);

    /* calculate the number of samples per frame */
    data.nsam = codec2_samples_per_frame(data.codec2);
    printf("number of samples per frame: %d\n", data.nsam);

    /* calculate the number of bits per frame */
    data.nbit = codec2_bits_per_frame(data.codec2);
    printf("number of bits: %d\n", data.nbit);

    /* calculate the number of bytes */
    data.nbyte = (data.nbit + 7) / 8;
    printf("number of bytes: %d\n", data.nbyte);

    data.buf = malloc ( data.nsam * sizeof(short) );
    data.bits = malloc (data.nbit * sizeof(char) );

    int i;
    for (i = 0; i < data.nsam; i++) { data.buf[i] = 0; }
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
              fuzzCallback,
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
    free(data.buf);
    free(data.bits);
    codec2_destroy(data.codec2);
    return 0;

error:
    Pa_Terminate();
    free(data.buf);
    free(data.bits);
    codec2_destroy(data.codec2);
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return -1;
}
