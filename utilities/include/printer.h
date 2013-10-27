#ifndef PRINTER_H
#define PRINTER_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Printer
{
  public:
  static void print(std::string description, vector<bool> &subject);
  static void print(std::string description, vector<int> &subject);
};

#endif
