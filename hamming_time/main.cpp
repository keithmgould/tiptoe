#include <iostream>
#include <math.h>
#include <sys/time.h>
#include <vector>
#include <stdlib.h>
#include "../utilities/include/hamming.h"
using namespace std;

int main()
{
  srand(time(NULL));
  const int parity_bit_size = 5;
  int data_bit_size = pow(2,parity_bit_size) - parity_bit_size - 1;
  vector<bool> input;
  vector<bool> output;

  for(int i=0;i<data_bit_size;i++)
  {
    input.push_back(rand() % 2 > 0);
  }
  Hamming h(parity_bit_size);

  timeval time;
  gettimeofday(&time, NULL);
  long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
  cout <<  "before: " << millis << endl;

  h.encode(input, output);

  gettimeofday(&time, NULL);
  millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
  cout <<  "after: " << millis << endl;
  return 0;
}
