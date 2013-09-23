#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include "../utilities/constants.cpp"
#include "../utilities/delta_finder.cpp"
#include "../utilities/demodulate.cpp"
#include "../utilities/extract.cpp"
#include "../utilities/reverse_transcode.cpp"
#include "../utilities/upsample.cpp"

#define SAMPLE_RATE  (32000)
#define FRAMES_PER_BUFFER (1280)

#define NUM_SECONDS     (1)
/* #define DITHER_FLAG     (paDitherOff) */
#define DITHER_FLAG     (0) /**/
/** Set to 1 if you want to capture the recording to a file. */
#define WRITE_TO_FILE   (1)

/* Select sample format. */
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"

using namespace std;

typedef struct
{
    int         frameIndex;                       //Index into sample array
    int         maxFrameIndex;
    float       *recordedSamples;                 // holds actual floating point audio samples.  not needed
    float       timeAfterLastBuffersLastCrossing; // used to keep track of the delta between buffers
    float       lastBuffersLastDelta;
    float       lastBuffersLastSample;
    vector < vector<bool> > bits;                 // hold the data for testing
    vector<bool> remainingBits;

}
paTestData;

void demodulator(const void * inputBuffer, paTestData * data, vector<bool>& bits)
{
  float * floatInputBuffer = (float *) inputBuffer;
  vector<float>inputSamples;
  if(data->lastBuffersLastSample != -1000)
  {
    inputSamples.push_back(data->lastBuffersLastSample);
  }
  inputSamples.insert(inputSamples.end(), floatInputBuffer, floatInputBuffer + FRAMES_PER_BUFFER);
  vector<float>deltas;
  if (data->lastBuffersLastDelta != -1000)
  {
    deltas.push_back(data->lastBuffersLastDelta);
  }
  DeltaFinder::Perform(inputSamples, FRAMES_PER_BUFFER, SAMPLE_RATE, deltas, &data->timeAfterLastBuffersLastCrossing);
  data->lastBuffersLastDelta = deltas.back();
  data->lastBuffersLastSample = floatInputBuffer[FRAMES_PER_BUFFER - 1];
  vector<bool> demodulatedBits;
  Demodulate::Perform(deltas, demodulatedBits);
  Extract extract(demodulatedBits, data->remainingBits);
  vector<bool> extractedBits;
  extract.perform(extractedBits, data->remainingBits);
  ReverseTranscode reverse_transcode(extractedBits, 48);
  reverse_transcode.perform(bits);

  // used for testing...
  if(bits.size() > 0) { data->bits.push_back(bits); }
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
        vector<bool> outputBits;
        demodulator(inputBuffer, data, outputBits);
        unsigned char *outputBytes = (unsigned char *) &outputBits;
        for( i=0; i<framesToCalc; i++ )
        {
            *wptr++ = *rptr++;
        }
    }
    data->frameIndex += framesToCalc;
    return finished;
}

/*******************************************************************/
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
    data.timeAfterLastBuffersLastCrossing = -1000;
    data.lastBuffersLastDelta = -1000;
    data.lastBuffersLastSample = -1000;
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

    inputParameters.device = 5; //Pa_GetDefaultInputDevice(); /* default input device */
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
    cout << "Finished" << endl;

    if(DEBUG_MODE > 0)
    {
      for(int i=0; i < data.bits.size(); i++)
      {
        for(int j=0; j < data.bits.at(i).size(); j++)
        {
          cout << data.bits.at(i).at(j);
        }
        cout << endl;
      }
    }

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


