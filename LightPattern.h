#ifndef LIGHTPATTERN_H
#define LIGHTPATTERN_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "LightComm.h"
#include "kitchen_lights.h"
typedef unsigned char byte;

#define LOOP_DELAY 20

class LightPattern
{
private:
  uint16_t pixelCount;
  Adafruit_NeoPixel *pStrip;
  
  void Wheel(byte WheelPos, byte *red, byte *green, byte *blue);
  
public:  
  LightPattern(uint16_t pixelCount, uint8_t pin);
  ~LightPattern();

  void SetAllLightsColor(pixelColor_t color);
  void SetRainbow();
};

#endif //LIGHTPATTERN_H

