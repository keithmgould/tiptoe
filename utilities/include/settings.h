#ifndef SETTINGS_H
#define SETTINGS_H

#include <iostream>

class Settings
{
  private:

  // private constructor => singleton pattern
  Settings() : debug_level(0) {}
  static Settings *settings;

  public:
  int debug_level;
  static Settings *getInstance();
};

#endif
