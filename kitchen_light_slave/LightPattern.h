#ifndef LIGHTPATTERN_H
#define LIGHTPATTERN_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "LightComm.h"
typedef unsigned char byte;

class LightPattern
{
private:
  int m_pixelCount;
  Adafruit_NeoPixel strip;
  
  void Wheel(byte WheelPos, byte *red, byte *green, byte *blue);
  
public:
  bool colorUpdate = true;
  
  LightPattern(int pixelCount, int pin);
  ~LightPattern();

  void SetAllLightsColor(pixelColor_t color);
  void SetRainbow();
};

#endif //LIGHTPATTERN_H
