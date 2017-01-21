/* Kitchen Lights Master Controller
 *  
 * Starting with the TMRh20 GettingStarted example to form the basis of the
 * effort to progressively build up the comms using the nRF24L01+.
 * This file will represent the Arduino which will send the light color(s)
 * and/or patterns.
 * 
 * Credit goes to ManiacBug for the RF24 library and TMRh20 for the library
 * improvements.
 * 
 * Known Arduino IDEs under which this compiles: 1.8.0
 * Known Arduino IDEs under which this does not compile: 1.6.5
 */

#include "LightComm.h"
#include "LightPattern.h"

#define DBG_PRINTS 1

//Prototype(s)
void readSerialCommand(void);

displayModes_t displayMode = COLOR_ONLY;
LightComm* lc;
LightPattern* lp;
pixelColor_t color; //= {255, 0, 0};
bool colorUpdate = true;

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, 6, NEO_GRB + NEO_KHZ800);

void setup()
{
//#ifdef DBG_PRINTS
  Serial.begin(115200);
  Serial.println("Kitchen Lights Master Arduino");
//#endif //DBG_PRINTS

  color.red = 255;
  color.green = 0;
  color.blue = 0;

  LightComm *lc = new LightComm(MASTER);
  LightPattern *lp = new LightPattern(60, 6);

//  strip.begin();
//  for (int i = 0; i < 60; i++)
//  {
//    strip.setPixelColor(i, strip.Color(255, 0, 0));
//  }
//  strip.show();
  Serial.println("end setup");
}

void loop()
{
  if (colorUpdate)
  {
    switch (displayMode)
    {
      case COLOR_ONLY:
        Serial.println("Setting color only");
        lp->SetAllLightsColor(color);
        Serial.println("1");
        lc->CommandColor(color);
        Serial.println("2");
        colorUpdate = false;
        Serial.println("3");
        break;
      case RAINBOW:
        Serial.println(F("Setting rainbow"));
        lp->SetRainbow();
        lc->CommandMode(displayMode);
        break;
      default:
        displayMode = COLOR_ONLY;
        break;
    }
  }
  // Wait a second
  delay(1000);

//TODO add a sleep here??
  readSerialCommand();

} // Loop

void readSerialCommand(void)
{
  if (Serial.available())
  {
    char c = toupper(Serial.read());
    switch (c)
    {
      case 'R':
        Serial.println(F("setting color to Red"));
        displayMode = COLOR_ONLY;
//        color = {255, 0, 0};
        color.red = 255;
        color.green = 0;
        color.blue = 0;
        colorUpdate = true;
        break;
      case 'G':
        Serial.println(F("setting color to Green"));
        displayMode = COLOR_ONLY;
//        color = {0, 255, 0};
        color.red = 0;
        color.green = 255;
        color.blue = 0;
        colorUpdate = true;
        break;
      case 'B':
        Serial.println(F("setting color to Blue"));
        displayMode = COLOR_ONLY;
//        color = {0, 0, 255};
        color.red = 0;
        color.green = 0;
        color.blue = 255;
        colorUpdate = true;
        break;
      case 'W':
        Serial.println(F("setting to Rainbow mode"));
        displayMode = RAINBOW;
        colorUpdate = true;
        break;
      default:
        Serial.println(F("Unknown command"));
        break;
    }
  }
}


