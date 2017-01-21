/* Kitchen Lights Slave Controller
 *  
 * Starting with the TMRh20 GettingStarted example to form the basis of the
 * effort to progressively build up the comms using the nRF24L01+.
 * This file will represent the Arduino(s) which will receive the light color(s)
 * and/or patterns.
 * 
 * Credit goes to ManiacBug for the RF24 library and TMRh20 for the library
 * improvements.
 */

#include "LightComm.h"
#include "LightPattern.h"

displayModes_t displayMode = COLOR_ONLY;
LightComm lc(SLAVE);
LightPattern lp(60, 6);
pixelColor_t color;
displayModes_t mode;

void setup()
{
#ifdef DBG_PRINTS
  Serial.begin(115200);
  Serial.println(F("Kitchen Lights Slave Arduino"));
#endif //DBG_PRINTS
  color = {0};
  mode = COLOR_ONLY;
}

void loop()
{  
  if(lc.LightUpdateQuery(&mode, &color))
  {
    switch(mode)
    {
      case COLOR_ONLY:
        lp.SetAllLightsColor(color);
        break;
      case RAINBOW:
        lp.SetRainbow();
        break;
      default:
        break;
    }
  }


} // Loop

