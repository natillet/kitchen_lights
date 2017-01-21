#include "LightPattern.h"

LightPattern::LightPattern(uint16_t pixelCount, uint8_t pin)
{
  m_pixelCount = pixelCount;
//  m_colorUpdate = true;
  // Parameter 1 = number of pixels in strip
  // Parameter 2 = Arduino pin number (most are valid)
  // Parameter 3 = pixel type flags, add together as needed:
  //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
  //   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
  //   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
  //   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
  //   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
  m_strip = new Adafruit_NeoPixel(m_pixelCount, pin, NEO_GRB + NEO_KHZ800);

  m_strip->begin();
  pixelColor_t color_green = {255, 255, 0};
  SetAllLightsColor(color_green);
  m_strip->show(); //init all pixels to off
}

LightPattern::~LightPattern()
{
  //destruct anything?
}

void LightPattern::SetAllLightsColor(pixelColor_t color)
{
  Serial.print("SetAllLightsColor: R ");
  Serial.print(color.red);
  Serial.print(" G ");
  Serial.print(color.green);
  Serial.print(" B ");
  Serial.println(color.blue);
  for (int i = 0; i < m_pixelCount; i++)
  {
    m_strip->setPixelColor(i, m_strip->Color(color.red, color.green, color.blue));
//    m_strip->setPixelColor(i, m_strip->Color(0, 255, 0));
  }
  m_strip->show();
}

void LightPattern::SetRainbow(void)
{
  static uint16_t j = 0;
  const int wait = 1;
  byte temp_red = 0;
  byte temp_green = 0;
  byte temp_blue = 0;

  for (uint16_t i = 0; i < m_pixelCount; i++)
  {
    Wheel(((i * 256 / m_pixelCount) + j) & 255, &temp_red, &temp_green, &temp_blue);
    m_strip->setPixelColor(i, m_strip->Color(temp_red, temp_green, temp_blue));
  }
  m_strip->show();
  delay(wait);
  j++;
  if (j >= 256)
  {
    j = 0;
  }
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
void LightPattern::Wheel(byte WheelPos, byte *red, byte *green, byte *blue) {
  WheelPos = 255 - WheelPos;
  byte WheelPosBy3 = WheelPos * 3;
  if (WheelPos < 85)
  {
    *red = 255 - WheelPosBy3;
    *green = 0;
    *blue = WheelPosBy3;
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    WheelPosBy3 = WheelPos * 3;
    *red = 0;
    *green = WheelPosBy3;
    *blue = 255 - WheelPosBy3;
  }
  else
  {
    WheelPos -= 170;
    WheelPosBy3 = WheelPos * 3;
    *red = WheelPosBy3;
    *green = 255 - WheelPosBy3;
    *blue = 0;
  }
}

