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
#define NUM_SECONDS     (1)
/* #define DITHER_FLAG     (paDitherOff) */
#define DITHER_FLAG     (0) /**/
/** Set to 1 if you want to capture the recording to a file. */
#define WRITE_TO_FILE   (1)

/* Select sample format. */
#if 1
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#endif

using namespace std;

typedef struct
{
    int         frameIndex;                     //Index into sample array
    int         maxFrameIndex;
    SAMPLE      *recordedSamples;               // holds actual floating point audio samples.  not needed
    SAMPLE      lastSampleFromPrevBuffer;       // helps with demodulation
    float       deltaAfterLastCrossing;         // used to keep track of the delta between buffers
    vector<bool> bits;                          // hold the data post demodulation

}
paTestData;

void demodulator(const void * inputBuffer, paTestData * data)
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
  Demodulate::Perform(deltas, data->bits);
}

static int recordCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    paTestData *data = (paTestData*)userData;
    const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
    SAMPLE *wptr = &data->recordedSamples[data->frameIndex];
    long framesToCalc;
    long i;
    int finished;
    unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;

    (void) outputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;

    if( framesLeft < framesPerBuffer )
    {
        framesToCalc = framesLeft;
        finished = paComplete;
    }
    else
    {
        framesToCalc = framesPerBuffer;
        finished = paContinue;
    }

    if( inputBuffer == NULL )
    {
        for( i=0; i<framesToCalc; i++ )
        {
            *wptr++ = SAMPLE_SILENCE;
        }
    }
    else
    {
        demodulator(inputBuffer, data);
        for( i=0; i<framesToCalc; i++ )
        {
            *wptr++ = *rptr++;
        }
    }
    data->frameIndex += framesToCalc;
    return finished;
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

    data.maxFrameIndex = numSamples = NUM_SECONDS * SAMPLE_RATE;
    data.frameIndex = 0;
    data.lastSampleFromPrevBuffer = -1000;
    data.deltaAfterLastCrossing = -1000;
    numBytes = numSamples * sizeof(SAMPLE);
    data.recordedSamples = (SAMPLE *) malloc( numBytes );
    if( data.recordedSamples == NULL )
    {
        printf("Could not allocate record array.\n");
        goto done;
    }
    for( i=0; i<numSamples; i++ ) data.recordedSamples[i] = 0;

    err = Pa_Initialize();
    if( err != paNoError ) goto done;

    inputParameters.device = 2; //Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default input device.\n");
        goto done;
    }
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    /* Record some audio. -------------------------------------------- */
    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL,                  /* &outputParameters, */
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              recordCallback,
              &data );
    if( err != paNoError ) goto done;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto done;
    printf("\n=== Now recording!!\n"); fflush(stdout);

    while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
    {
        Pa_Sleep(1000);
    }
    if( err < 0 ) goto done;

    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto done;

    /* Measure maximum peak amplitude. */
    max = 0;
    average = 0.0;
    for( i=0; i<numSamples; i++ )
    {
        val = data.recordedSamples[i];
        if( val < 0 ) val = -val; /* ABS */
        if( val > max )
        {
            max = val;
        }
        average += val;
    }

    average = average / (double)numSamples;

    printf("sample max amplitude = "PRINTF_S_FORMAT"\n", max );
    printf("sample average = %lf\n", average );

    /* Write recorded data to a file. */
#if WRITE_TO_FILE
    {
        FILE  *fid;
        fid = fopen("recorded.raw", "wb");
        if( fid == NULL )
        {
            printf("Could not open file.");
        }
        else
        {
            // based on 0x00 0xFF 0x5D ->  010101010101010110101010101010100110011010100110
            fwrite( data.recordedSamples, sizeof(SAMPLE), numSamples, fid );
            fclose( fid );
            printf("Wrote data to 'recorded.raw'\n");
        }

        ofstream bitFile;
        bitFile.open ("data.txt");
        vector<bool>::iterator it;
        for(it = data.bits.begin(); it != data.bits.end(); it++)
        {
          bitFile << *it;
        }
        bitFile.close();
        printf("Wrote data to 'data.txt'\n");
    }
#endif


done:
    Pa_Terminate();
    if( data.recordedSamples )       /* Sure it is NULL or valid. */
        free( data.recordedSamples );
    if( err != paNoError )
    {
        fprintf( stderr, "An error occured while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        err = 1;          /* Always return 0 or 1, but no other return codes. */
    }
    return err;
}


