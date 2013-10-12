#ifndef TRANSCODE_H
#define TRANSCODE_H

#include <iostream>
#include <vector>
#include <bitset>
#include "constants.h"
#include "../include/hamming.h"

using namespace std;

class Transcode
{
  public:
  Transcode() : hamming(4) {}
  void perform(vector<bool> &rawBits, vector<bool> &transcodedBits);
  private:
  Hamming hamming;
  void add_preamble_bits(vector<bool> &transcodedBits);
  void add_transcoded_bits(vector<bool> &rawBits, vector<bool> &transcodedBits);
  void add_buffer_bits(vector<bool> &transcodedBits);
  void add_hamming_bits(vector<bool> &transcodedBits);
};

#endif
