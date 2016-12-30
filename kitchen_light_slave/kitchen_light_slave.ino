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

#include <SPI.h>
#include "RF24.h"
#include <Adafruit_NeoPixel.h>

#define DBG_PRINTS 1
#define PIN 6
#define NUM_LEDS 40

typedef struct pixelValue {
  byte red;
  byte blue;
  byte green;
  byte num;
} pixelValue_t;

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
pixelValue_t rcvPixel = {0};
bool colorUpdate = false;

void setup()
{
#ifdef DBG_PRINTS
  Serial.begin(115200);
  Serial.println(F("Kitchen Lights Slave Arduino"));
#endif //DBG_PRINTS
  
  radio.begin();

  // Set the PA Level. RF24_PA_MAX is default.
//  radio.setPALevel(RF24_PA_LOW);
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
  radio.setPayloadSize(sizeof(pixelValue_t));
  
  // Start the radio listening for data
  radio.startListening();

  //Start the Neopixel strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop()
{
  radio.startListening();
//  unsigned long got_time;
  
  if (radio.available())
  {
#ifdef DBG_PRINTS
    Serial.println(F("Data Available"));
#endif //DBG_PRINTS
                                                                  // Variable for the received timestamp
    while (radio.available())                                     // While there is data ready
    {
      radio.read(&rcvPixel, sizeof(pixelValue_t));               // Get the payload
    }
#ifdef DBG_PRINTS
    Serial.println(F("Received Value"));
#endif //DBG_PRINTS

    strip.setPixelColor(rcvPixel.num, strip.Color(rcvPixel.red, rcvPixel.green, rcvPixel.blue));
    strip.show();
  }

} // Loop

