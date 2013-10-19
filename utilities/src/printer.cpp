#include "../include/printer.h"

void Printer::print(std::string description, vector<bool> &subject)
{
  cout << endl << description << " (size " << subject.size() << "): ";
  for(int i=0; i< subject.size();i++)
  {
    cout << subject.at(i);
  }
  cout << endl;
}
