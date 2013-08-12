/*
 * Transmit is the lowest level in the stack,
 * taking a raw bit stream and converting to
 * analog using the IncDec algorithm
 */

#include <stdio.h>
#include <iostream>
#include <csignal>
#include <fstream>
#include <vector>
#include <bitset>
#include <math.h>
#include "portaudio.h"
#include "transcode.cpp"

#define SAMPLE_RATE   (6000)
#define FRAMES_PER_BUFFER  (512)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

using namespace std;

#define LOW_TABLE_SIZE    (12)
#define BASE_TABLE_SIZE   (9)
#define HIGH_TABLE_SIZE   (6)
#define LOW  (0)
#define BASE (1)
#define HIGH (2)

typedef struct
{
    float base_sine[BASE_TABLE_SIZE];
    float high_sine[HIGH_TABLE_SIZE];
    float low_sine[LOW_TABLE_SIZE];
    int mode;                           // The frequency we are on: base, high or low
    int phase;                          // Where in the sinusoid we are
}
transmitData;

// SO DUH: Sample output must equal sample input count.
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

    // faux input -- some random bytes
    vector<unsigned char> faux;

    faux.push_back ( 0x00 );
    faux.push_back ( 0xFF );
    faux.push_back ( 0xEE );
    faux.push_back ( 0xA5 );
    faux.push_back ( 0xB2 );
    faux.push_back ( 0xDD );

    vector<bool> transcodedBits;
    Transcode::Perform(faux, transcodedBits);
    vector<bool>::iterator bitIterator = transcodedBits.begin();
    cout << endl;
    for(bitIterator; bitIterator != transcodedBits.end(); bitIterator++)
    {
      cout << *bitIterator;
    }
    cout << endl;
    bitIterator = transcodedBits.begin();
    bool nextSinusoid = false;
    for( int i=0; i<framesPerBuffer; i++ )
    {
        if (data->mode == HIGH){
          *out++ = data->high_sine[data->phase];
          data->phase += 1;
          if(data->phase >= HIGH_TABLE_SIZE) { nextSinusoid = true;}
        }else if(data->mode == BASE){
          *out++ = data->base_sine[data->phase];
          data->phase += 1;
          if(data->phase >= BASE_TABLE_SIZE) { nextSinusoid = true;}
        }else{
          *out++ = data->low_sine[data->phase];
          data->phase += 1;
          if(data->phase >= LOW_TABLE_SIZE) { nextSinusoid = true;}
        }
        if(nextSinusoid)
        {
          if(bitIterator >= transcodedBits.end())
          {
            cout <<  endl << "all out of bits yo" << endl;
            return 1;
          }
          nextSinusoid = false;
          data->phase = 0;
          if(bitIterator == transcodedBits.begin())
          {
            data->mode = 1;
          }
          if(*bitIterator == 1)
          {
            data->mode += 1;
          }else{
            data->mode -= 1;
          }
          cout << *bitIterator << ":" << data->mode << ",";
          if(data->mode > 2 || data->mode < 0)
          {
            cout << endl << "data-> mode is out of range.  It is now: " << data->mode << endl;
            cout << "occurred at: " << i << endl;
            return 1;
          }
          bitIterator += 1;
        }
    }
    return paContinue;
}

/*******************************************************************/
int main(void)
{
    PaStreamParameters outputParameters;
    PaStream *stream;
    PaError err;
    transmitData data;
    int i;

    /* initialise sinusoidal wavetable */
    for( i=0; i<BASE_TABLE_SIZE; i++ )
    {
        data.base_sine[i] = (float) cos( ((double)i/(double)BASE_TABLE_SIZE) * M_PI * 2.0);
    }

    for( i=0; i<HIGH_TABLE_SIZE; i++ )
    {
        data.high_sine[i] = (float) cos( ((double)i/(double)HIGH_TABLE_SIZE) * M_PI * 2.0);
    }

    for( i=0; i<LOW_TABLE_SIZE; i++ )
    {
        data.low_sine[i] = (float) cos( ((double)i/(double)LOW_TABLE_SIZE) * M_PI * 2.0);
    }

    data.phase = 0;
    data.mode = BASE;

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


