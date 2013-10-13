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
  Transcode() : hamming(7) {}
  void perform(vector<bool> &rawBits, vector<bool> &transcodedBits);
  private:
  Hamming hamming;
  void add_preamble_to_finalPacket(vector<bool> &finalPacket);
  void transcode_bits(vector<bool> &rawBits, vector<bool> &transcodedBits);
  void add_buffer_bits(vector<bool> &transcodedBits, int total_size);
  void determine_parity_bits(vector<bool> &transcodedBits, vector<bool> &parityBits);
  void transcode_parity_bits(vector<bool> &parityBits, vector<bool> &transcodedParityBits);
  void add_bits_to_finalPacket(vector<bool> &bits, vector<bool> &finalPacket);
};


#endif
