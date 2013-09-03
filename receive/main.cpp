#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include "../utilities/delta_finder.cpp"
#include "../utilities/demodulate.cpp"

#define SAMPLE_RATE  (32000)
#define FRAMES_PER_BUFFER (1280)

/* #define DITHER_FLAG     (paDitherOff) */
#define DITHER_FLAG     (0) /**/

typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)

using namespace std;

typedef struct
{
    // used to ensure we don't miss a crossing defined by last frame of
    // previous buffer and first frame of current buffer
    SAMPLE      lastSampleFromPrevBuffer;

    // used to keep track of the delta between last crossing of previous buffer
    // and first crossing of current buffer
    float       deltaAfterLastCrossing;
}
paTestData;

void demodulator(const void * inputBuffer, paTestData * data, vector<bool>& bits)
{
  float * floatInputBuffer = (float *) inputBuffer;
  vector<float>deltas;
  // add the last sample, in case it was below zero and the first sample of the
  // current buffer is above zero.  Otherwise we miss a crossing point.
  if (data->lastSampleFromPrevBuffer != -1000)
  {
    deltas.push_back(data->lastSampleFromPrevBuffer);
  }
  DeltaFinder::Perform(floatInputBuffer, FRAMES_PER_BUFFER, SAMPLE_RATE, deltas, &data->deltaAfterLastCrossing);
  data->lastSampleFromPrevBuffer = deltas.back();
  Demodulate::Perform(deltas, bits);
}

static int remoteToLocalCallback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData )
{
    paTestData *data = (paTestData*)userData;
    const SAMPLE *rptr = (const SAMPLE*)inputBuffer;

    (void) outputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    vector<bool> bits;
    if( inputBuffer == NULL )
    {

    }else{
      demodulator(inputBuffer, data, bits);
    }
    return paContinue;
}

/*******************************************************************/
int main(void);
int main(void)
{
    PaStreamParameters  inputParameters,
                        outputParameters;
    PaStream*           stream;
    PaError             err = paNoError;
    paTestData          data;
    int                 i;
    int                 numSamples;
    int                 numBytes;
    SAMPLE              max, val;
    double              average;

    data.lastSampleFromPrevBuffer = -1000;
    data.deltaAfterLastCrossing = -1000;

    err = Pa_Initialize();
    if( err != paNoError ) goto done;

    inputParameters.device = 2; //Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default input device.\n");
        goto done;
    }
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL,                  /* &outputParameters, */
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              remoteToLocalCallback,
              &data );
    if( err != paNoError ) goto done;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto done;

    cout << "Hit ENTER to stop program." << endl;
    getchar();
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto done;

    Pa_Terminate();
    printf("Finished.\n");

done:
    Pa_Terminate();
    if( err != paNoError )
    {
        fprintf( stderr, "An error occured while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        err = 1;          /* Always return 0 or 1, but no other return codes. */
    }
    return err;
}


