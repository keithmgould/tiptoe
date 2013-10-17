#ifndef TRANSCODER_H
#define TRANSCODER_H

#include <iostream>
#include <vector>
#include <bitset>
#include "constants.h"

using namespace std;

class Transcoder
{
  public:
  static void Transcode(vector<bool> &rawBits, vector<bool> &transcodedBits);
};
#endif
