#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"
#include "codec2.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include "../utilities/constants.cpp"
#include "../utilities/convert.cpp"
#include "../utilities/delta_finder.cpp"
#include "../utilities/demodulate.cpp"
#include "../utilities/extract.cpp"
#include "../utilities/reverse_transcode.cpp"
#include "../utilities/upsample.cpp"


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
 * downstample to 8kHz, which yields 320 frames per buffer
 */
#define SAMPLE_RATE  (32000)
#define FRAMES_PER_BUFFER (1280)

/* #define DITHER_FLAG     (paDitherOff) */
#define DITHER_FLAG     (0) /**/

#define SAMPLE_SILENCE  (0.0f)

using namespace std;

typedef struct
{
    // used to store codec2 instantiation
    CODEC2 *codec2;

    int         frameIndex;                       //Index into sample array
    int         maxFrameIndex;
    float      *recordedSamples;                 // holds actual floating point audio samples.  not needed
    float       timeAfterLastBuffersLastCrossing; // used to keep track of the delta between buffers
    float       lastBuffersLastDelta;
    float       lastBuffersLastSample;
    vector < vector<bool> > bits;                 // hold the data for testing
    vector<bool> remainingBits;
}
paTestData;

void demodulator(const void * inputBuffer, paTestData * data, vector<bool>& outputBits)
{
  float * floatInputBuffer = (float *) inputBuffer;
  vector<float>inputSamples;
  if(data->lastBuffersLastSample != -1000) { inputSamples.push_back(data->lastBuffersLastSample); }
  inputSamples.insert(inputSamples.end(), floatInputBuffer, floatInputBuffer + FRAMES_PER_BUFFER);
  vector<float>deltas;
  if (data->lastBuffersLastDelta != -1000) { deltas.push_back(data->lastBuffersLastDelta); }

  // Determines the length of each sinusoid
  DeltaFinder::Perform(inputSamples, FRAMES_PER_BUFFER, SAMPLE_RATE, deltas, &data->timeAfterLastBuffersLastCrossing);
  data->lastBuffersLastDelta = deltas.back();
  data->lastBuffersLastSample = floatInputBuffer[FRAMES_PER_BUFFER - 1];
  vector<bool> demodulatedBits;

  // converts the sinusoid length changes to 1s and 0s
  Demodulate::Perform(deltas, demodulatedBits);

  // Looks for the preambles and takes out the data between the preambles
  Extract extract(demodulatedBits, data->remainingBits);
  vector<bool> extractedBits;
  extract.perform(extractedBits, data->remainingBits);

  // Reverses the transcoding process, leaving real data
  ReverseTranscode reverse_transcode(extractedBits, 53);
  vector<bool> untranscodedBits;
  reverse_transcode.perform(untranscodedBits);

  // Ensures we have continuity by examining the bufferCounter
  outputBits.assign(untranscodedBits.begin()+5, untranscodedBits.end());
}

static int remoteToLocalCallback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData )
{
    paTestData *data = (paTestData*)userData;
    const float *rptr = (const float*)inputBuffer;
    short *out = (short*)outputBuffer;
    short compressedOutput[320];

    (void) timeInfo;
    (void) statusFlags;
    if( inputBuffer == NULL )
    {
      cout << "emptyness..." << endl;
    }else{
      vector<bool> outputBits;
      demodulator(inputBuffer, data, outputBits);
      if(outputBits.size() == 48)
      {
        // for(int i=0;i<48;i++)
        // {
          // cout << outputBits.at(i);
        // }
        // cout << endl;
        unsigned char outputBytes[6];
        Convert::BitsToUnsignedChar(outputBits, outputBytes, 6);
        codec2_decode(data->codec2, compressedOutput, outputBytes);
        Upsample::Perform(compressedOutput, out, 320);
      }else{
        cout << "bad packet" << endl;
      }
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
    float               max, val;
    double              average;

    err = Pa_Initialize();
    if( err != paNoError ) goto done;

    /* create a pointer to the codec states */
    data.codec2 = codec2_create(CODEC2_MODE);
    data.timeAfterLastBuffersLastCrossing = -1000;
    data.lastBuffersLastDelta = -1000;
    data.lastBuffersLastSample = -1000;

    inputParameters.device = 3; //Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default input device.\n");
        goto done;
    }
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
      fprintf(stderr,"Error: No default output device.\n");
      goto done;
    }
    outputParameters.channelCount = 1;
    outputParameters.sampleFormat = paInt16;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              &outputParameters,
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


