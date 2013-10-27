#include "../include/settings.h"

Settings *Settings::getInstance()
{
  if(settings == NULL)
  {
    settings = new Settings();
  }
  return settings;
}

Settings* Settings::settings = NULL;
