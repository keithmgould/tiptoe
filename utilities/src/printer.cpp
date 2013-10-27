#include "../include/printer.h"

/*
 Some friendly printing tools for debugging
*/

void Printer::print(std::string description, vector<bool> &subject)
{
  cout << description << " (size " << subject.size() << "): ";
  for(int i=0; i< subject.size();i++)
  {
    cout << subject.at(i);
  }
  cout << endl;
}

void Printer::print(std::string description, vector<int> &subject)
{
  cout << description << " (size " << subject.size() << "): ";
  for(int i=0; i< subject.size();i++)
  {
    cout << subject.at(i);
  }
  cout << endl;
}
