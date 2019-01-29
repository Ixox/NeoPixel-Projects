/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/

#ifndef LEDLAMP_H
#define LEDLAMP_H

#include <Arduino.h>
#include "FastLED.h"

#define NEOPIXEL_PIN    12

#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define LEDS_PER_LEVEL 8
#define NUMBER_OF_LEVELS 37
#define NUMBER_OF_LEDS   (LEDS_PER_LEVEL * NUMBER_OF_LEVELS)
#define NUMBER_OF_LEDS_FIRE NUMBER_OF_LEDS


/*
*/
enum LAMP_MODE
{
  MODE_FIRST = 0,
  MODE_STATIC = 0,
  MODE_COLOR_CHANGE2,
  MODE_FIRE,
  MODE_NOISE,
  MODE_SPARKLING,
  MODE_LAST,
  MODE_COLOR_CHANGE // INCLUDED IN MODE_COLOR_CHANGE2
};

typedef union {
    struct {
        uint8_t energyMode: 2;
        uint8_t currentEnergy: 3;
        uint8_t nextEnergy: 3;
    };
    uint8_t value;
} LampParam;



class LedLamp
{
public:
  void init();
  void nextMode();
  uint8_t render();
  void setBrightness(uint8_t brigtness);
  void setParam1(uint8_t param1);
  void setParam2(uint8_t param2);
  void setParam3(uint8_t param3);
  void setParam4(uint8_t param4);

  void saveToEEPROM();
  void loadFromEEPROM();

private:
  uint8_t renderStatic();
  uint8_t renderColorChange();
  uint8_t renderColorChange2();
  uint8_t renderFire();
  uint8_t renderNoise();
  uint8_t renderSparkling();
  inline void setColor(uint32_t index, CRGB color);

  inline void reduce(byte &aByte, byte aAmount, byte aMin = 0);
  inline void increase(byte &aByte, byte aAmount, byte aMax = 255);
  uint16_t random(uint16_t aMinOrMax, uint16_t aMax = 0);


  uint8_t m_mode;
  uint8_t m_brightness;
  uint8_t m_param1;
  uint8_t m_param2;
  uint8_t m_param3;
  uint8_t m_param4;
  uint8_t m_hue;
  uint16_t m_cpt;

  CRGB leds[NUMBER_OF_LEDS + 1];
  
  LampParam m_param[NUMBER_OF_LEDS_FIRE]; 

  // Noise variable
  uint16_t noisex;
  uint16_t noisey;
  uint16_t noisez;
  CRGBPalette16 m_currentPalette;
  uint8_t m_currentPaletteValue;

};

#endif
