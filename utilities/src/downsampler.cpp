void Downsampler::Perform(short inputSamples[], short outputSamples[], int inputLength, int downsampleRate)
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

// TODO: refactor: do this with anonymous pointers (or something).  this repetition is stupid...
void Downsampler::Perform(float inputSamples[], float outputSamples[], int inputLength, int downsampleRate)
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
