#include <iostream>
#include <vector>
#include "../utilities/constants.cpp"
#include "../utilities/transcode.cpp"

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
