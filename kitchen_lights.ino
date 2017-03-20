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
#define REFRESH_RATE 250  //*LOOP_DELAY = 250 * 20ms = 5s

//Prototype(s)
void readSerialCommand(void);

displayModes_t displayMode = COLOR_ONLY;
LightComm* lc;
LightPattern* lp;
pixelColor_t color;
bool colorUpdate = true;
long refreshRateCounter = 0;


void setup()
{
#ifdef DBG_PRINTS
  Serial.begin(115200);
  Serial.println("Kitchen Lights Master Arduino");
#endif //DBG_PRINTS

  lc = new LightComm(MASTER);
  lp = new LightPattern(60, 6);
  
  color = {255, 0, 0};
  displayMode = COLOR_ONLY;

  Serial.println("end setup");
}

void loop()
{
  switch (displayMode)
  {
    case COLOR_ONLY:
      if (colorUpdate)
      {
        Serial.println("Setting color only");
        if (lc->CommandColor(color))
        {
          colorUpdate = false;
        }
        lp->SetAllLightsColor(color);
      }
      break;
    case RAINBOW:
      if (colorUpdate)
      {
        Serial.println(F("Setting rainbow"));
        if (lc->CommandMode(displayMode))
        {
          colorUpdate = false;
        }
      }
      lp->SetRainbow();
      break;
    default:
      displayMode = COLOR_ONLY;
      break;
  }
  // Wait a moment (impacts rainbox speed)
  delay(LOOP_DELAY);

  readSerialCommand();

  if (refreshRateCounter++ > REFRESH_RATE)
  {
    refreshRateCounter = 0;
    colorUpdate = true;
  }

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


