#include <iostream>

// Utilities
#include "utility.cpp"

// Audio!!
#include "portaudio.h"

// Crypto!!
#include "cryptopp/aes.h"
#include "cryptopp/hex.h"
#include "cryptopp/osrng.h"
#include "cryptopp/modes.h"
#include "cryptopp/secblock.h"

// #define SAMPLE_RATE         (44100)
#define SAMPLE_RATE         (8000)
#define PA_SAMPLE_TYPE      paFloat32
#define FRAMES_PER_BUFFER   (128)

// Convenience!!
using namespace std;
using namespace CryptoPP;

typedef float SAMPLE;
const int BYTES_PER_BUFFER = 4 * FRAMES_PER_BUFFER;

typedef struct
{
  CTR_Mode< AES >::Encryption encryptor;
  CTR_Mode< AES >::Decryption decryptor;
}
paTestData;


void encryptBuffer (const void * plainBuffer, float * encryptedBuffer, paTestData * data)
{
  float * floatPlainBuffer = (float *) plainBuffer;
  float floatRecoveredBuffer[FRAMES_PER_BUFFER];
  byte byteArrayPlainBuffer[BYTES_PER_BUFFER];
  byte byteArrayEncryptedBuffer[BYTES_PER_BUFFER];
  byte byteArrayRecoveredBuffer[BYTES_PER_BUFFER];

  // 1) Convert from float array to Byte Array
  Utility::FloatArrayToByteArray(floatPlainBuffer, FRAMES_PER_BUFFER, byteArrayPlainBuffer);

  // 2) Encrypt
  ArraySource( byteArrayPlainBuffer, BYTES_PER_BUFFER, true,
    new StreamTransformationFilter( data->encryptor,
      new ArraySink(byteArrayEncryptedBuffer, BYTES_PER_BUFFER )
    )
  );

  // 3) Decrypt
  ArraySource( byteArrayEncryptedBuffer, BYTES_PER_BUFFER, true,
    new StreamTransformationFilter ( data->decryptor,
      new ArraySink( byteArrayRecoveredBuffer, BYTES_PER_BUFFER)
    )
  );

  // 4) Convert from byte array to float array
  Utility::ByteArrayToFloatArray(byteArrayRecoveredBuffer, BYTES_PER_BUFFER, floatRecoveredBuffer);

  // 5) Use recovered encrypted data as ouput
  for(int i = 0; i < FRAMES_PER_BUFFER; i++ )
  {
    encryptedBuffer[i] = floatRecoveredBuffer[i];
  }

}

static int encryptCallback( const void *inputBuffer,
                            void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{

  paTestData *data = (paTestData*)userData;
  SAMPLE *out = (SAMPLE*)outputBuffer;
  const SAMPLE *in = (const SAMPLE*)inputBuffer;
  unsigned int i;
  float encryptedBuffer[framesPerBuffer];
  (void) timeInfo; /* Prevent unused variable warnings. */
  (void) statusFlags;

  if( inputBuffer == NULL )
  {
      for( i=0; i<framesPerBuffer; i++ )
      {
          *out++ = 0;  /* silent */
      }
  }
  else
  {
      encryptBuffer(inputBuffer, encryptedBuffer, data);
      for( i=0; i<framesPerBuffer; i++ )
      {
          *out++ = encryptedBuffer[i] * 2;
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


    /*************************/
    // Encryption Stuff

    AutoSeededRandomPool rng;
    SecByteBlock key(AES::DEFAULT_KEYLENGTH);
    rng.GenerateBlock( key, key.size() );

    byte ctr[AES::BLOCKSIZE];
    rng.GenerateBlock(ctr, sizeof(ctr));

    CTR_Mode< AES >::Encryption e;
    e.SetKeyWithIV( key, key.size(), ctr);

    CTR_Mode< AES >::Decryption d;
    d.SetKeyWithIV( key, key.size(), ctr );

    data.encryptor = e;
    data.decryptor = d;

    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice) {
      fprintf(stderr,"Error: No default input device.\n");
      goto error;
    }
    inputParameters.channelCount = 1;       /* stereo input */
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
      fprintf(stderr,"Error: No default output device.\n");
      goto error;
    }
    outputParameters.channelCount = 1;       /* mono output */
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
              encryptCallback,
              &data );
    if( err != paNoError ) goto error;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;

    cout << "Hit ENTER to stop program." << endl;
    getchar();
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;

    cout << "Finished." << endl;
    Pa_Terminate();
    return 0;

error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return -1;
}

