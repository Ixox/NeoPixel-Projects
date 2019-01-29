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

template<int nPots> class LedControl
{
  public:
    LedControl(uint8_t pinButton, bool inverse = false);
    void init();         // Must be called after contructor
    void setPotPin(uint8_t pot, uint8_t potPin);
    void updateValues(); // To call in main loop before calling any other function bellow
    bool buttonClick();
    bool buttonClick1Second();
    bool hasNewPotValue(uint8_t pot);
    bool hasNewPotValueButtonPushed(uint8_t pot);
    uint8_t getPotValue(uint8_t pot);
    void reinitPotThreshold(uint8_t pot);

  private:
    uint8_t getCurrentPotValue(uint8_t pot);
    uint8_t m_pinButton;
    uint8_t m_pinPot[nPots];
    uint8_t m_potValueThreshold[nPots];
    unsigned long m_buttonPushStartTime;
    uint8_t m_potValueLast[nPots];
    bool m_potValueChanging[nPots];
    uint8_t m_potValue[nPots];
    uint8_t m_curentPotValue[nPots];
    bool m_pushButtonPressed;
    bool m_pushButtonPressedLast;
    unsigned long m_lastUpdate;
    bool m_potChangedWhileButtonPressed;
    bool m_longPressedDone;
    bool m_inverse;
};

#endif