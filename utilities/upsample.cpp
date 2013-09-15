class Upsample
{
  public:
  static void Perform(short inputSamples[], short outputSamples[], int inputLength)
  {
    for(int i=0; i < inputLength - 1; i++)
    {
      outputSamples[ i*4 + 0 ] = (inputSamples[i]*4)/4 + (inputSamples[i+1]*0)/4;
      outputSamples[ i*4 + 1 ] = (inputSamples[i]*3)/4 + (inputSamples[i+1]*1)/4;
      outputSamples[ i*4 + 2 ] = (inputSamples[i]*2)/4 + (inputSamples[i+1]*2)/4;
      outputSamples[ i*4 + 3 ] = (inputSamples[i]*1)/4 + (inputSamples[i+1]*3)/4;
    }
    outputSamples[(inputLength-1)*4] = inputSamples[inputLength - 1];
  }
};
