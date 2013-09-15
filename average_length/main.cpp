#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "../utilities/constants.cpp"
#include "../utilities/transcode.cpp"

/*
 * Simple script to determine the average length of transcoded bits
 * (Cause I suck at stats)
 */
int main()
{
  srand(time(NULL));
  int times = 10000;
  int avg_length = 0;
  int gt90 = 0;
  vector<bool> dataBits;
  vector<bool> transcodedBits;
  for(int i = 0; i< times; i++)
  {
    transcodedBits.clear();
    dataBits.clear();
    // choose 48 random bits
    for(int j=0; j<48;j++)
    {
      dataBits.push_back(rand() % 2 > 0);
      // cout << dataBits.back();
    }
    // cout << endl;

    Transcode::Perform(dataBits, transcodedBits);
    avg_length += transcodedBits.size();
    if(transcodedBits.size() > 90)
    {
      gt90++;
      cout<< "gt90 is: " << transcodedBits.size() << endl;
    }
  }
  cout << "avg:" << (float) avg_length / (float) times << endl;
  cout << "gt90: " << gt90 << endl;

  return 0;
}
