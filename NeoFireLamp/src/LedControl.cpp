/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/

#include "LedControl.h"
template<int nPots>
LedControl<nPots>::LedControl(uint8_t pinButton, bool inverse)
{
  m_pinButton = pinButton;
  m_inverse = inverse;
}

template<int nPots>
void LedControl<nPots>::init()
{
  pinMode(m_pinButton, INPUT_PULLUP);
  pinMode(13, OUTPUT);

  m_potChangedWhileButtonPressed = false;
  m_longPressedDone = false;

  m_pushButtonPressed = false;
  m_pushButtonPressedLast = false;
  
}

template<int nPots>
void LedControl<nPots>::setPotPin(uint8_t pot, uint8_t potPin) {
  if (pot < nPots) {
    m_pinPot[pot] = potPin;
    m_potValueChanging[pot] = false;
    pinMode(m_pinPot[pot], INPUT);
    m_potValue[pot] = getCurrentPotValue(pot);
    m_potValueThreshold[pot] = m_potValue[pot];
    m_curentPotValue[pot] = m_potValue[pot];
  }
}


template<int nPots>
uint8_t LedControl<nPots>::getCurrentPotValue(uint8_t pot)
{
  if (m_inverse) {
    return 255 - (analogRead(m_pinPot[pot]) >> 2);
  } else {
    return analogRead(m_pinPot[pot]) >> 2;
  }
}

template<int nPots>
void LedControl<nPots>::updateValues()
{
  m_lastUpdate = millis();

  m_pushButtonPressedLast = m_pushButtonPressed;
  m_pushButtonPressed = digitalRead(m_pinButton) > 0 ? false : true;

  for (int p = 0; p < nPots; p++) {
    m_curentPotValue[p] = getCurrentPotValue(p);
    // Change more thant 16 ? then we update pot value
    if (!m_potValueChanging[p])
    {
      if (abs(m_curentPotValue[p] - m_potValueThreshold[p]) > 16)
      {
        m_potValueChanging[p] = true;
      }
    }

    if (m_potValueChanging[p])
    {
      m_potValue[p] = m_curentPotValue[p];
    }
  }

  if (!m_pushButtonPressedLast && m_pushButtonPressed)
  {
    m_buttonPushStartTime = m_lastUpdate;
    m_potChangedWhileButtonPressed = false;
    m_longPressedDone = false;
  }

  if (m_pushButtonPressedLast != m_pushButtonPressed)
  {
    for (int p = 0; p < nPots; p++) {
      m_potValueChanging[p] = false;
      m_potValueThreshold[p] = m_curentPotValue[p];
    }
  }
}

template<int nPots>
bool LedControl<nPots>::buttonClick()
{
  return m_pushButtonPressedLast && !m_pushButtonPressed && !m_potChangedWhileButtonPressed && ((m_lastUpdate - m_buttonPushStartTime) < 1000);
}

template<int nPots>
bool LedControl<nPots>::buttonClick1Second()
{
  if (!m_longPressedDone && m_pushButtonPressed && !m_potChangedWhileButtonPressed && ((m_lastUpdate - m_buttonPushStartTime) > 2000))
  {
    m_longPressedDone = true;
    return true;
  }
  return false;
}

template<int nPots>
bool LedControl<nPots>::hasNewPotValue(uint8_t pot)
{
  return !m_pushButtonPressed && m_potValueChanging[pot];
}

template<int nPots>
bool LedControl<nPots>::hasNewPotValueButtonPushed(uint8_t pot)
{
  bool toReturn = m_pushButtonPressed && m_potValueChanging[pot];
  if (toReturn)
  {
    m_potChangedWhileButtonPressed = true;
  }
  return toReturn;
}

template<int nPots>
uint8_t LedControl<nPots>::getPotValue(uint8_t pot)
{
  digitalWrite(13, HIGH);
  return m_potValue[pot];
}

template<int nPots>
void LedControl<nPots>::reinitPotThreshold(uint8_t pot)
{
  m_potValueChanging[pot] = false;
  m_potValueThreshold[pot] = m_curentPotValue[pot];
}

