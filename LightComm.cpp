#include "LightComm.h"


LightComm::LightComm(RadioType_t controllerType)
{
  m_pRadio = new RF24(7,8); // Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8

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
  if (m_pRadio != NULL)
  {
    delete m_pRadio;
  }
}

bool LightComm::CommandMode(displayModes_t mode)
{
  bool result = false;
  
  m_currMode = mode;
  result = SendCommand();
  
  return result;
}

bool LightComm::CommandColor(pixelColor_t color)
{
  bool result = false;
  m_currColor.red = color.red;
  m_currColor.green = color.green;
  m_currColor.blue = color.blue;
  
#ifdef DBG_PRINTS
  Serial.print(F("Commanding color: "));
  Serial.print(m_currColor.red);
  Serial.print(F(" "));
  Serial.print(m_currColor.green);
  Serial.print(F(" "));
  Serial.println(m_currColor.blue);
#endif //DBG_PRINTS

	m_currMode = COLOR_ONLY;
	result = SendCommand();
 
  return result;
}

bool LightComm::SendCommand()
{
  bool result = false;
  
  lightCommand_t command = {0};
  command.mode = m_currMode;
  command.color.red = m_currColor.red;
  command.color.green = m_currColor.green;
  command.color.blue = m_currColor.blue;
  
  if (!m_pRadio->write(&command, sizeof(lightCommand_t)))
  {
    result = false;
#ifdef DBG_PRINTS
    Serial.println(F("Send failed"));
#endif //DBG_PRINTS
  }
  else
  {
    result = true;
#ifdef DBG_PRINTS
    Serial.print(F("Sending color: "));
    Serial.print(command.color.red);
    Serial.print(F(" "));
    Serial.print(command.color.green);
    Serial.print(F(" "));
    Serial.println(command.color.blue);
#endif //DBG_PRINTS
  }

  delayMicroseconds(MIN_TX_DELAY_US);

  return result;
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
//    m_currColor = command.color;
//    *color = command.color;
    m_currColor.red = command.color.red;
    m_currColor.green = command.color.green;
    m_currColor.blue = command.color.blue;
    (*color).red = command.color.red;
    (*color).green = command.color.green;
    (*color).blue = command.color.blue;
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

