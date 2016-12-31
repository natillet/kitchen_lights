/* Kitchen Lights Master Controller
 *  
 * Starting with the TMRh20 GettingStarted example to form the basis of the
 * effort to progressively build up the comms using the nRF24L01+.
 * This file will represent the Arduino which will send the light color(s)
 * and/or patterns.
 * 
 * Credit goes to ManiacBug for the RF24 library and TMRh20 for the library
 * improvements.
 */

#include <SPI.h>
#include "RF24.h"
#include <Adafruit_NeoPixel.h>

//Prototype(s)
void readSerialCommand(void);
void setAllRed(void);
void setAllGreen(void);
void setAllBlue(void);
void setRainbow(void);
void sendAndSetPixelUpdate(byte red, byte green, byte blue, byte num, int delay_ms);
void Wheel(byte WheelPos, byte *red, byte *green, byte *blue);

#define DBG_PRINTS 1
#define PIN 6
#define NUM_LEDS 25
#define DELAY_MS_SINGLE_COLOR 10
#define DELAY_MS_UPDATING_COLOR 1

typedef struct pixelValue {
  byte red;
  byte green;
  byte blue;
  byte num;
  byte show;
} pixelValue_t;

typedef enum {
  RED = 0,
  GREEN,
  BLUE,
  RAINBOW,
  NUM_MODES
} displayModes_t;

RF24 radio(7,8); // Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8
byte addresses[][6] = {"1Node","2Node"};
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);
bool colorUpdate = true;
displayModes_t displayMode = RED;

void setup()
{
#ifdef DBG_PRINTS
  Serial.begin(115200);
  Serial.println(F("Kitchen Lights Master Arduino"));
#endif //DBG_PRINTS
  
  radio.begin();

  // Set the PA Level. RF24_PA_MAX is default.
//  radio.setPALevel(RF24_PA_LOW);
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[1]);
  
  radio.stopListening();

    //Start the Neopixel strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

}

void loop()
{
  if (colorUpdate)
  {
    switch (displayMode)
    {
      case RED:
        setAllRed();
        colorUpdate = false;
        break;
      case GREEN:
        setAllGreen();
        colorUpdate = false;
        break;
      case BLUE:
        setAllBlue();
        colorUpdate = false;
        break;
      case RAINBOW:
        setRainbow();
        break;
      default:
        displayMode = RED;
        break;
    }
  }
  // Wait a second
//  delay(1000);

  readSerialCommand();

} // Loop



void readSerialCommand(void)
{
  if (Serial.available())
  {
    char c = toupper(Serial.read());
    if (c == 'R')
    {
      Serial.println(F("setting color to Red"));
      displayMode = RED;
      colorUpdate = true;
    }
    else if (c == 'B')
    {
      Serial.println(F("setting color to Blue"));
      displayMode = BLUE;
      colorUpdate = true;
    }
    else if (c == 'G')
    {
      Serial.println(F("setting color to Green"));
      displayMode = GREEN;
      colorUpdate = true;
    }
    else if (c == 'W')
    {
      Serial.println(F("setting to Rainbow mode"));
      displayMode = RAINBOW;
      colorUpdate = true;
    }
    else
    {
      Serial.println(F("Unknown command"));
    }
  }
}

void setAllRed(void)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    if (i < (NUM_LEDS-1)) //don't show until we've set them all
    {
      sendAndSetPixelUpdate(255, 0, 0, i, 0, DELAY_MS_SINGLE_COLOR);
    }
    else
    {
      sendAndSetPixelUpdate(255, 0, 0, i, 1, DELAY_MS_SINGLE_COLOR);
    }
  }
}

void setAllGreen(void)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    if (i < (NUM_LEDS-1)) //don't show until we've set them all
    {
      sendAndSetPixelUpdate(0, 255, 0, i, 0, DELAY_MS_SINGLE_COLOR);
    }
    else
    {
      sendAndSetPixelUpdate(0, 255, 0, i, 1, DELAY_MS_SINGLE_COLOR);
    }
  }
}

void setAllBlue(void)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    if (i < (NUM_LEDS-1)) //don't show until we've set them all
    {
      sendAndSetPixelUpdate(0, 0, 255, i, 0, DELAY_MS_SINGLE_COLOR);
    }
    else
    {
      sendAndSetPixelUpdate(0, 0, 255, i, 1, DELAY_MS_SINGLE_COLOR);
    }
  }
}

void setRainbow(void)
{
  static uint16_t j = 0;
  const int wait = 1;
  byte temp_red = 0;
  byte temp_green = 0;
  byte temp_blue = 0;

  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    Wheel(((i * 256 / NUM_LEDS) + j) & 255, &temp_red, &temp_green, &temp_blue);
    sendAndSetPixelUpdate(temp_red, temp_green, temp_blue, i, 1, DELAY_MS_UPDATING_COLOR);
  }
  delay(wait);
  j++;
  if (j >= 256)
  {
    j = 0;
  }
}

void sendAndSetPixelUpdate(byte red, byte green, byte blue, byte num, byte show, int delay_ms)
{
  pixelValue_t sendPixel = {red, green, blue, num, show};
  
  if (!radio.write(&sendPixel, sizeof(pixelValue_t)))
  {
#ifdef DBG_PRINTS
    Serial.println(F("Send failed"));
#endif //DBG_PRINTS
  }
  else
  {
#ifdef DBG_PRINTS    
//    Serial.println(F("Send succeeded"));
    Serial.print(sendPixel.red);
    Serial.print(F(" "));
    Serial.print(sendPixel.green);
    Serial.print(F(" "));
    Serial.print(sendPixel.blue);
    Serial.print(F(" "));
    Serial.print(sendPixel.num);
    Serial.print(F(" "));
    Serial.println(sendPixel.show);
#endif //DBG_PRINTS
  }

  strip.setPixelColor(sendPixel.num, strip.Color(sendPixel.red, sendPixel.green, sendPixel.blue));
  if (show)
  {
    strip.show();
  }
  delay(delay_ms);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
void Wheel(byte WheelPos, byte *red, byte *green, byte *blue) {
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

