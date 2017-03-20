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

//#define IS_SLAVE

#include "kitchen_lights.h"
#include "LightComm.h"
#include "LightPattern.h"

#define REFRESH_RATE 250  //*LOOP_DELAY = 250 * 20ms = 5s

//Prototype(s)
void readSerialCommand(void);
void slave_loop(void);
void master_loop(void);

//Globals
displayModes_t displayMode = COLOR_ONLY;
LightComm* lc;
LightPattern* lp;
pixelColor_t color;
bool colorUpdate = true;
long refreshRateCounter = 0;
#ifdef IS_SLAVE
RadioType_t radio = SLAVE;
#else //IS_MASTER
RadioType_t radio = MASTER;
#endif //IS_SLAVE

void setup()
{
  Serial.begin(115200);
  DBG_PRINTLN("0");
#ifdef IS_SLAVE
  Serial.println("Kitchen Lights Slave Arduino");
#else //IS_MASTER
  Serial.println("Kitchen Lights Master Arduino");
#endif //IS_SLAVE

  DBG_PRINTLN("1");
  lc = new LightComm(radio);
  DBG_PRINTLN("2");
  lp = new LightPattern(60, 6);
  DBG_PRINTLN("3");
  
  color = {255, 0, 0};
  displayMode = COLOR_ONLY;

  DBG_PRINTLN("end setup");
}

void loop()
{
#ifdef IS_SLAVE
  slave_loop();
#else
  master_loop();
#endif
}

void slave_loop()
{
  displayModes_t temp_mode;
  pixelColor_t temp_color;
  
  if (lc->LightUpdateQuery(&temp_mode, &temp_color))
  {
    DBG_PRINTLN("received light update");
    displayMode = temp_mode;
    color = temp_color;
    colorUpdate = true;
  }

  switch(displayMode)
  {
    case COLOR_ONLY:
      if (colorUpdate)
      {
        lp->SetAllLightsColor(color);
        colorUpdate = false;
      }
      break;
    case RAINBOW:
      lp->SetRainbow();
      break;
    default:
      break;
  }
  // Wait a moment (impacts rainbow speed)
  delay(LOOP_DELAY);  

}

void master_loop()
{
  switch (displayMode)
  {
    case COLOR_ONLY:
      if (colorUpdate)
      {
        DBG_PRINTLN("Setting color only");
        if (lc->SendColorCommand(color))
        {
          colorUpdate = false;
        }
        lp->SetAllLightsColor(color);
      }
      break;
    case RAINBOW:
      if (colorUpdate)
      {
        DBG_PRINTLN(F("Setting rainbow"));
        if (lc->SendModeCommand(displayMode))
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
  // Wait a moment (impacts rainbow speed)
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
        DBG_PRINTLN(F("setting color to Red"));
        displayMode = COLOR_ONLY;
//        color = {255, 0, 0};
        color.red = 255;
        color.green = 0;
        color.blue = 0;
        colorUpdate = true;
        break;
      case 'G':
        DBG_PRINTLN(F("setting color to Green"));
        displayMode = COLOR_ONLY;
//        color = {0, 255, 0};
        color.red = 0;
        color.green = 255;
        color.blue = 0;
        colorUpdate = true;
        break;
      case 'B':
        DBG_PRINTLN(F("setting color to Blue"));
        displayMode = COLOR_ONLY;
//        color = {0, 0, 255};
        color.red = 0;
        color.green = 0;
        color.blue = 255;
        colorUpdate = true;
        break;
      case 'H':
        DBG_PRINTLN(F("setting color to White"));
        displayMode = COLOR_ONLY;
//        color = {0, 0, 255};
        color.red = 127;
        color.green = 127;
        color.blue = 127;
        colorUpdate = true;
        break;
      case 'W':
        DBG_PRINTLN(F("setting to Rainbow mode"));
        displayMode = RAINBOW;
        colorUpdate = true;
        break;
      default:
        DBG_PRINTLN(F("Unknown command"));
        break;
    }
  }
}


