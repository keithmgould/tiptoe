#ifndef PACKER_H
#define PACKER_H

#include <iostream>
#include <vector>
#include <bitset>
#include "constants.h"
#include "../include/hamming.h"
#include "../include/transcoder.h"

using namespace std;

class Packer
{
  public:
  Packer() : hamming(7) {}
  void build(vector<bool> &rawBits, vector<bool> &transcodedBits);
  private:
  Hamming hamming;
  void add_preamble_to_packet(vector<bool> &packet);
  void add_buffer_bits(vector<bool> &transcodedBits, int total_size);
  void determine_parity_bits(vector<bool> &transcodedBits, vector<bool> &parityBits);
  void transcode_parity_bits(vector<bool> &parityBits, vector<bool> &transcodedParityBits);
  void add_bits_to_packet(vector<bool> &bits, vector<bool> &packet);
};


#endif
