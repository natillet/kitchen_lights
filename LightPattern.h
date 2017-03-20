#ifndef LIGHTPATTERN_H
#define LIGHTPATTERN_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "LightComm.h"
typedef unsigned char byte;

#define LOOP_DELAY 20

class LightPattern
{
private:
  uint16_t m_pixelCount;
  Adafruit_NeoPixel *m_pStrip;
  
  void Wheel(byte WheelPos, byte *red, byte *green, byte *blue);
  
public:
//  bool m_colorUpdate = true;
  
  LightPattern(uint16_t pixelCount, uint8_t pin);
  ~LightPattern();

  void SetAllLightsColor(pixelColor_t color);
  void SetRainbow();
};

#endif //LIGHTPATTERN_H

