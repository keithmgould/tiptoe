/*
 * Transmit is the lowest level in the stack,
 * taking a raw bit stream and converting to
 * analog using the IncDec algorithm
 */

#include <stdio.h>
#include <iostream>
#include <math.h>
#include "portaudio.h"

#define SAMPLE_RATE   (6000)
#define FRAMES_PER_BUFFER  (256)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

using namespace std;

#define LOW_TABLE_SIZE   (9)
#define HIGH_TABLE_SIZE   (6)
typedef struct
{
    float low_sine[LOW_TABLE_SIZE];
    float high_sine[HIGH_TABLE_SIZE];
    int mode;
    int phase;
}
transmitData;

static int transmitCallback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
    transmitData *data = (transmitData*)userData;
    float *out = (float*)outputBuffer;

    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) inputBuffer;

    for( int i=0; i<framesPerBuffer; i++ )
    {
        if (data->mode == 0){
          *out++ = data->high_sine[data->phase];
          data->phase += 1;
          if(data->phase >= HIGH_TABLE_SIZE) { data->phase = 0; data->mode = 1;}
        }else{
          *out++ = data->low_sine[data->phase];
          data->phase += 1;
          if(data->phase >= LOW_TABLE_SIZE) { data->phase = 0; data->mode = 0;}
        }
    }
    return paContinue;
}

/*
 * This routine is called by portaudio when playback is done.
 */
static void StreamFinished( void* userData )
{
   transmitData *data = (transmitData *) userData;
   printf( "Stream Completed\n");
}

/*******************************************************************/
int main(void);
int main(void)
{
    PaStreamParameters outputParameters;
    PaStream *stream;
    PaError err;
    transmitData data;
    int i;

    /* initialise sinusoidal wavetable */
    for( i=0; i<LOW_TABLE_SIZE; i++ )
    {
        data.low_sine[i] = (float) cos( ((double)i/(double)LOW_TABLE_SIZE) * M_PI * 2.0);
    }


    for( i=0; i<(HIGH_TABLE_SIZE * 2); i++ )
    {
        data.high_sine[i] = (float) cos( ((double)i/(double)HIGH_TABLE_SIZE) * M_PI * 2.0);
    }

    data.phase = 0;
    data.mode = 0;

    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    outputParameters.device =2; // Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
      fprintf(stderr,"Error: No default output device.\n");
      goto error;
    }
    outputParameters.channelCount = 1;       /* mono output */
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
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

    err = Pa_SetStreamFinishedCallback( stream, &StreamFinished );
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


