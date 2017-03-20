#include "LightComm.h"


LightComm::LightComm(RadioType_t controllerType)
{
  this->pRadio = new RF24(7,8); // Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8

  this->pRadio->begin();
  // Set the PA Level. RF24_PA_MAX is default.
//  this->pRadio->setPALevel(RF24_PA_LOW);
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  if (controllerType == MASTER)
  {
    this->pRadio->openWritingPipe(addresses[0]);  //uses pipe 0
    this->pRadio->openReadingPipe(1,addresses[1]);
    this->pRadio->stopListening();
  }
  else
  {
    this->pRadio->openWritingPipe(addresses[1]);
    this->pRadio->openReadingPipe(1,addresses[0]);  //uses pipe 0
    this->pRadio->startListening();
  }

//  this->pRadio->printDetails();
  this->command = {0};
}

LightComm::~LightComm()
{
  if (this->pRadio != NULL)
  {
    delete this->pRadio;
  }
}

bool LightComm::SendModeCommand(displayModes_t mode)
{
  bool result = false;
  
  this->command.mode = mode;
  result = SendCommand();
  
  return result;
}

bool LightComm::SendColorCommand(pixelColor_t color)
{
  bool result = false;
  this->command.color.red = color.red;
  this->command.color.green = color.green;
  this->command.color.blue = color.blue;
  
  DBG_PRINT(F("Commanding color: "));
  DBG_PRINT(this->command.color.red);
  DBG_PRINT(F(" "));
  DBG_PRINT(this->command.color.green);
  DBG_PRINT(F(" "));
  DBG_PRINTLN(this->command.color.blue);

	this->command.mode = COLOR_ONLY;
	result = SendCommand();
 
  return result;
}

bool LightComm::SendCommand()
{
  bool result = false;
    
  if (!this->pRadio->write(&command, sizeof(lightCommand_t)))
  {
    result = false;
    DBG_PRINTLN(F("Send failed"));
  }
  else
  {
    result = true;

    DBG_PRINT(F("Sending color: "));
    DBG_PRINT(this->command.color.red);
    DBG_PRINT(F(" "));
    DBG_PRINT(this->command.color.green);
    DBG_PRINT(F(" "));
    DBG_PRINTLN(this->command.color.blue);
  }

  delayMicroseconds(MIN_TX_DELAY_US);

  return result;
}

bool LightComm::LightUpdateQuery(displayModes_t *mode, pixelColor_t *color)
{
  bool resultIsUpdateLights = false;
  if (this->pRadio->available())
  {
    while (this->pRadio->available())                                     // While there is data ready
    {
      this->pRadio->read(&this->command, sizeof(lightCommand_t));               // Get the payload
      DBG_PRINTLN("waiting for dataz");
    }

    resultIsUpdateLights = true;
    (*color).red = this->command.color.red;
    (*color).green = this->command.color.green;
    (*color).blue = this->command.color.blue;
    *mode = this->command.mode;
    
    DBG_PRINT(F("Received data: "));
    DBG_PRINT(this->command.color.red);
    DBG_PRINT(F(" "));
    DBG_PRINT(this->command.color.green);
    DBG_PRINT(F(" "));
    DBG_PRINTLN(this->command.color.blue);
  }
  else
  {
    DBG_PRINTLN("radio not available");
  }
  return resultIsUpdateLights;
}

