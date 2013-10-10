#ifndef TRANSCODE_H
#define TRANSCODE_H

#include <vector>
#include <iostream>
using namespace std;

class Transcode
{
  public:
  static void AddPreamble(vector<bool> &rawBits, vector<bool> &transcodedBits);
  static void Perform(vector<bool> &rawBits, vector<bool> &transcodedBits);
};

#endif
