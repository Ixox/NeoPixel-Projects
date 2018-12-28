/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/

#ifndef LEDSPOT_H
#define LEDSPOT_H

#include <Adafruit_NeoPixel.h>

enum LedAnimation
{
    ANIM_0 = 0,
    ANIM_1,
    ANIM_2,
    ANIM_3,
    ANIM_4,
    ANIM_FINISHED
};

class LedSpot
{
  public:
    LedSpot(uint16_t numberOfPixels, uint8_t pin);
    void setAllColor(uint32_t c);
    uint32_t getColorFrom8bitsValue(uint8_t value, bool stopAtwhite = true);
    void setBrightness(uint8_t brightness);
    uint8_t getBrightness() { return m_brightness; }
    void show();
    void incLastUpdatedLed();
    uint8_t getLastUpdatedOpositeLed();
    void animationStart(LedAnimation anim);
    void animationNextStep();
    void setAnimationSpeed(uint8_t animationSpeed);
    void nextAnimation();
    uint32_t getRandomColor();

    void loadStateFromEEPROM(int index);
    void saveStateToEEPROM(int index);
    void acknowledge(uint32_t color);

  private:
    Adafruit_NeoPixel *m_leds;
    uint8_t m_numberOfPixels;
    uint8_t m_brightness;
    LedAnimation m_animationType;
    int16_t m_animationStep;
    int16_t m_animationSubStep;
    uint8_t m_animationSpeed;
    uint8_t m_lastUpdatedLed;
};

#endif