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
LightComm* lc;
LightPattern* lp;
pixelColor_t color;
displayModes_t mode;
bool refresh = false;

void setup()
{
#ifdef DBG_PRINTS
  Serial.begin(115200);
  Serial.println(F("Kitchen Lights Slave Arduino"));
#endif //DBG_PRINTS

  lc = new LightComm(SLAVE);
  lp = new LightPattern(60, 6);
  
  color = {0};
  mode = COLOR_ONLY;
  
  Serial.println("end setup");
}

void loop()
{
  displayModes_t temp_mode;
  pixelColor_t temp_color;
  
  if (lc->LightUpdateQuery(&temp_mode, &temp_color))
  {
    Serial.println("received light update");
    mode = temp_mode;
    color = temp_color;
    refresh = true;
  }

  switch(mode)
  {
    case COLOR_ONLY:
      if (refresh)
      {
        lp->SetAllLightsColor(color);
        refresh = false;
      }
      break;
    case RAINBOW:
      lp->SetRainbow();
      break;
    default:
      break;
  }
  // Wait a moment (impacts rainbox speed)
  delay(LOOP_DELAY);  

} // Loop

