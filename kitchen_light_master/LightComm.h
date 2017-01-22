#ifndef LIGHTCOMM_H
#define LIGHTCOMM_H

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
typedef unsigned char byte;

#define DBG_PRINTS 1
#define MIN_TX_DELAY_US 1000

typedef enum {
  MASTER,
  SLAVE
} RadioType_t;

typedef struct pixelColor {
  byte red;
  byte green;
  byte blue;
} pixelColor_t;

typedef struct lightCommand {
  byte mode;
  pixelColor_t color;
} lightCommand_t;

typedef enum {
  COLOR_ONLY = 0,
  RAINBOW,
  NUM_MODES
} displayModes_t;

static const byte addresses[][6] = {"1Node","2Node"};

class LightComm
{
private:
  RF24* m_pRadio;
  pixelColor_t m_currColor;
  displayModes_t m_currMode;

  bool SendCommand();
  
public:
  LightComm(RadioType_t);
  ~LightComm();

  bool CommandMode(displayModes_t mode);
  bool CommandColor(pixelColor_t color);
  bool LightUpdateQuery(displayModes_t *mode, pixelColor_t *color);
};


#endif //LIGHTCOMM_H

