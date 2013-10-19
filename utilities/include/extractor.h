#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <vector>
#include <utility>
using namespace std;

class Extractor
{
  public:
  vector<bool> totalRawBits;

  Extractor (vector<bool> &newRawBits, vector<bool> &previousRawBits); // constructor
  void perform(vector<bool> &extractedBits, vector<bool> &remainingRawBits);
  pair <int, int> findPreambles();
};

#endif
