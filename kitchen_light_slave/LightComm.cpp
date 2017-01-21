#include "LightComm.h"


LightComm::LightComm(RadioType_t controllerType)
{
  RF24 *m_pRadio = new RF24(7,8); // Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8

  m_pRadio->begin();
  // Set the PA Level. RF24_PA_MAX is default.
//  m_pRadio->setPALevel(RF24_PA_LOW);
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  if (controllerType == MASTER)
  {
    m_pRadio->openWritingPipe(addresses[0]);  //uses pipe 0
    m_pRadio->openReadingPipe(1,addresses[1]);
  }
  else
  {
    m_pRadio->openWritingPipe(addresses[1]);  //uses pipe 0
    m_pRadio->openReadingPipe(1,addresses[0]);
  }

  m_currColor.red = 0;
  m_currColor.green = 0;
  m_currColor.blue = 0;
  m_currMode = COLOR_ONLY;
}

LightComm::~LightComm()
{
  //destruct anything?
}

void LightComm::CommandMode(displayModes_t mode)
{
  if (mode != m_currMode)
  {
    m_currMode = mode;
	SendCommand(m_currMode, m_currColor);
  }
}

void LightComm::CommandColor(pixelColor_t color)
{
  if (color.red != m_currColor.red || 
       color.green != m_currColor.green || 
	   color.blue != m_currColor.blue)
  {
    // m_currColor.red = color.red;
    // m_currColor.green = color.green;
    // m_currColor.blue = color.blue;
	m_currColor = color;
	m_currMode = COLOR_ONLY;
	SendCommand(m_currMode, m_currColor);
  }
}

void LightComm::SendCommand(displayModes_t mode, pixelColor_t color)
{
  // m_currColor.red = color.red;
  // m_currColor.green = color.green;
  // m_currColor.blue = color.blue;
  m_currColor = color;
  m_currMode = mode;
  lightCommand_t command = {mode, color};
  
  if (!m_pRadio->write(&command, sizeof(lightCommand_t)))
  {
  #ifdef DBG_PRINTS
    Serial.println(F("Send failed"));
  #endif //DBG_PRINTS
  }
  else
  {
  #ifdef DBG_PRINTS
    Serial.print(color.red);
    Serial.print(F(" "));
    Serial.print(color.green);
    Serial.print(F(" "));
    Serial.println(color.blue);
  #endif //DBG_PRINTS
  }

  delayMicroseconds(MIN_TX_DELAY_US);
}

bool LightComm::LightUpdateQuery(displayModes_t *mode, pixelColor_t *color)
{
  bool resultIsUpdateLights = false;
  lightCommand_t command;
  if (m_pRadio->available())
  {
    while (m_pRadio->available())                                     // While there is data ready
    {
      m_pRadio->read(&command, sizeof(lightCommand_t));               // Get the payload
    }

    resultIsUpdateLights = true;
    m_currColor = command.color;
    *color = command.color;
    m_currMode = command.mode;
    *mode = command.mode;
    
#ifdef DBG_PRINTS
    Serial.print(F("Received data: "));
    Serial.print(m_currColor.red);
    Serial.print(F(" "));
    Serial.print(m_currColor.green);
    Serial.print(F(" "));
    Serial.println(m_currColor.blue);
#endif //DBG_PRINTS
  }
  return resultIsUpdateLights;
}
