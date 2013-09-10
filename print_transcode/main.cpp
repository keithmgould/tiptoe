#include <iostream>
#include <vector>
#include "../utilities/constants.cpp"
#include "../utilities/transcode.cpp"

/*
 * Simple script to print out what the transcode of data would be.
 *
 * Use this in case you quickly want to see what bytes would
 * transcode to...
 */
int main()
{

  unsigned char faux[6] = { 0xD3, 0xA9, 0x5D, 0x2D, 0xBC, 0x94 };
  vector<bool> transcodedBits;
  Transcode::Perform(faux, transcodedBits, 6);
  vector<bool>::iterator it = transcodedBits.begin();
  for(;it != transcodedBits.end(); it++)
  {
    cout << *it;
  }
  cout << endl << endl;
  return 0;
}
