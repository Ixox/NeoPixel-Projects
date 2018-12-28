/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/

#include "LedControl.h"

LedControl::LedControl(uint8_t pinButton, uint8_t pinPot)
{
    m_pinButton = pinButton;
    m_pinPot = pinPot;
}

void LedControl::init()
{

    pinMode(m_pinButton, INPUT_PULLUP);
    // DEBUG
    pinMode(13, OUTPUT);

    m_potChangedWhileButtonPressed = false;
    m_longPressedDone = false;

    m_pushButtonPressed = false;
    m_pushButtonPressedLast = false;
    m_potValueChanging = false;

    m_potValue = getCurrentPotValue();
    m_potValueThreshold = m_potValue;
    m_curentPotValue = m_potValue;
}

uint8_t LedControl::getCurrentPotValue()
{
    return analogRead(m_pinPot) >> 2;
}

void LedControl::updateValues()
{
    m_lastUpdate = millis();

    m_pushButtonPressedLast = m_pushButtonPressed;

    m_pushButtonPressed = digitalRead(m_pinButton) > 0 ? false : true;
    m_curentPotValue = getCurrentPotValue();

    // Change more thant 10 ? then we update pot value
    if (!m_potValueChanging)
    {
        if (abs(m_curentPotValue - m_potValueThreshold) > 16)
        {
            m_potValueChanging = true;
            digitalWrite(13, HIGH);
        }
    }

    if (m_potValueChanging)
    {
        m_potValue = m_curentPotValue;
    }

    if (!m_pushButtonPressedLast && m_pushButtonPressed)
    {
        m_buttonPushStartTime = m_lastUpdate;
        m_potChangedWhileButtonPressed = false;
        m_longPressedDone = false;
    }

    if (m_pushButtonPressedLast != m_pushButtonPressed)
    {
        m_potValueChanging = false;
        m_potValueThreshold = m_curentPotValue;
    }
}

bool LedControl::buttonClick()
{
    return m_pushButtonPressedLast && !m_pushButtonPressed && !m_potChangedWhileButtonPressed && ((m_lastUpdate - m_buttonPushStartTime) < 1000);
}

bool LedControl::buttonClick1Second()
{
    if (!m_longPressedDone && m_pushButtonPressed && !m_potChangedWhileButtonPressed && ((m_lastUpdate - m_buttonPushStartTime) > 2000))
    {
        m_longPressedDone = true;
        return true;
    }
    return false;
}

bool LedControl::hasNewPotValue()
{
    return !m_pushButtonPressed && m_potValueChanging;
}

bool LedControl::hasNewPotValueButtonPushed()
{
    bool toReturn = m_pushButtonPressed && m_potValueChanging;
    if (toReturn)
    {
        m_potChangedWhileButtonPressed = true;
    }
    return toReturn;
}

uint8_t LedControl::getPotValue()
{
    return m_potValue;
}

void LedControl::reinitPotThreshold()
{
    m_potValueChanging = false;
    m_potValueThreshold = m_curentPotValue;
    digitalWrite(13, LOW);
}
