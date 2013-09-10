class Downsample
{
  public:
  static void Perform(short inputSamples[], short outputSamples[], int inputLength, int downsampleRate)
  {
    int outputIndex = 0;
    for(int i = 0; i < inputLength; i++)
    {
      if((i % downsampleRate) == 0)
      {
        outputSamples[outputIndex++] = inputSamples[i];
      }
    }
  }
};
