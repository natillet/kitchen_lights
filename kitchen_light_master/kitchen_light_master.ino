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
pixelValue_t sendPixel = {0};

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

  //Start the Neopixel strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
#ifdef DBG_PRINTS    
  Serial.println(F("Now sending"));
#endif //DBG_PRINTS
}

void loop()
{
  sendPixel.red = 255;
  sendPixel.blue = 128;
  sendPixel.green = 32;
  sendPixel.num = 3;
  
  radio.stopListening();                                    // First, stop listening so we can talk.

//  unsigned long start_time = micros();                             // Take the time, and send it.  This will block until complete
  
  if (!radio.write(&sendPixel, sizeof(pixelValue_t)))
  {
#ifdef DBG_PRINTS
    Serial.println(F("Send failed"));
#endif //DBG_PRINTS
  }
  else
  {
#ifdef DBG_PRINTS
    Serial.println(F("Send succeeded"));
#endif //DBG_PRINTS
  }

  strip.setPixelColor(sendPixel.num, strip.Color(sendPixel.red, sendPixel.green, sendPixel.blue));
  strip.show();
      
//  radio.startListening();                                    // Now, continue listening
//  
//  unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
//  boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
//  
//  while (!radio.available())                                 // While nothing is received
//  {
//    if (micros() - started_waiting_at > 200000)              // If waited longer than 200ms, indicate timeout and exit while loop
//    {
//        timeout = true;
//        break;
//    }      
//  }
//      
//  if (timeout)
//  {
//#ifdef DBG_PRINTS
//      Serial.println(F("Failed, response timed out."));      // Describe the results
//#endif //DBG_PRINTS
//  }
//  else
//  {
//      unsigned long got_time;                                 // Grab the response, compare, and send to debugging spew
//      radio.read(&got_time, sizeof(unsigned long));
//      unsigned long end_time = micros();
//      
//      // Spew it
//#ifdef DBG_PRINTS
//      Serial.print(F("Sent "));
//      Serial.print(start_time);
//      Serial.print(F(", Got response "));
//      Serial.print(got_time);
//      Serial.print(F(", Round-trip delay "));
//      Serial.print(end_time-start_time);
//      Serial.println(F(" microseconds"));
//#endif //DBG_PRINTS
//  }

  // Try again 1s later
  delay(1000);

/****************** Change Roles via Serial Commands ***************************/
//  if ( Serial.available() )
//  {
//    char c = toupper(Serial.read());
//    if ( c == 'T' && role == 0 ){      
//      Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
//      role = 1;                  // Become the primary transmitter (ping out)
//    
//   }else
//    if ( c == 'R' && role == 1 ){
//      Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));      
//       role = 0;                // Become the primary receiver (pong back)
//       radio.startListening();
//       
//    }
//  }


} // Loop

