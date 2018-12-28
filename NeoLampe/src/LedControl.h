/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/

#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include <Arduino.h>

/*
 * Takes care of reading the states of the button and the rotary potentiometre.
 * Sets the correct internal values from them. 
*/

class LedControl
{
  public:
    LedControl(uint8_t pinButton, uint8_t pinPot);
    void init();         // Must be called after contructor
    void updateValues(); // To call in main loop before calling any other function bellow
    bool buttonClick();
    bool buttonClick1Second();
    bool hasNewPotValue();
    bool hasNewPotValueButtonPushed();
    uint8_t getPotValue();
    void reinitPotThreshold();

  private:
    uint8_t getCurrentPotValue();
    uint8_t m_pinButton;
    uint8_t m_pinPot;
    uint8_t m_potValueThreshold;
    unsigned long m_buttonPushStartTime;
    uint8_t m_potValueLast;
    bool m_potValueChanging;
    uint8_t m_potValue;
    uint8_t m_curentPotValue;
    bool m_pushButtonPressed;
    bool m_pushButtonPressedLast;
    unsigned long m_lastUpdate;
    bool m_potChangedWhileButtonPressed;
    bool m_longPressedDone;
};

#endif