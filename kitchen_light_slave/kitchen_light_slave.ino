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

#define DBG_PRINTS 1

bool radioNumber = 1; //0 will reflect master, 1 will reflect slave(s)
RF24 radio(7,8); // Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8
byte addresses[][6] = {"1Node","2Node"};
bool role = 0; // Used to control whether this node is sending or receiving

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
  if (radioNumber)
  {
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }
  else
  {
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
  
  // Start the radio listening for data
  radio.startListening();
  Serial.println("WORKING end setup");
}

void loop()
{
/****************** Ping Out Role ***************************/  
  if (role == 1)
  {
    radio.stopListening();                                    // First, stop listening so we can talk.
    
#ifdef DBG_PRINTS    
    Serial.println(F("Now sending"));
#endif //DBG_PRINTS

    unsigned long start_time = micros();                             // Take the time, and send it.  This will block until complete
     if (!radio.write(&start_time, sizeof(unsigned long)))
     {
#ifdef DBG_PRINTS
       Serial.println(F("failed"));
#endif //DBG_PRINTS
     }
        
    radio.startListening();                                    // Now, continue listening
    
    unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
    boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
    
    while (!radio.available())                                 // While nothing is received
    {
      if (micros() - started_waiting_at > 200000)              // If waited longer than 200ms, indicate timeout and exit while loop
      {
          timeout = true;
          break;
      }      
    }
        
    if (timeout)
    {
#ifdef DBG_PRINTS
        Serial.println(F("Failed, response timed out."));      // Describe the results
#endif //DBG_PRINTS
    }
    else
    {
        unsigned long got_time;                                 // Grab the response, compare, and send to debugging spew
        radio.read(&got_time, sizeof(unsigned long));
        unsigned long end_time = micros();
        
        // Spew it
#ifdef DBG_PRINTS
        Serial.print(F("Sent "));
        Serial.print(start_time);
        Serial.print(F(", Got response "));
        Serial.print(got_time);
        Serial.print(F(", Round-trip delay "));
        Serial.print(end_time-start_time);
        Serial.println(F(" microseconds"));
#endif //DBG_PRINTS
    }

    // Try again 1s later
    delay(1000);
  }

/****************** Pong Back Role ***************************/
  if (role == 0)
  {
    unsigned long got_time;
    
    if( radio.available())
    {
                                                                    // Variable for the received timestamp
      while (radio.available())                                     // While there is data ready
      {
        radio.read(&got_time, sizeof(unsigned long));               // Get the payload
      }
     
      radio.stopListening();                                        // First, stop listening so we can talk   
      radio.write(&got_time, sizeof(unsigned long));                // Send the final one back.      
      radio.startListening();                                       // Now, resume listening so we catch the next packets.
#ifdef DBG_PRINTS    
      Serial.print(F("Sent response "));
      Serial.println(got_time);
#endif //DBG_PRINTS
   }
 }

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

