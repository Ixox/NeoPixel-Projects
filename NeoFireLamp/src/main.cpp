/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/

#include <Arduino.h>

#include <FastLED.h>

#include "LedControl.h"
#include "LedLamp.h"

#define PIN_PUSHBUTTON A0
#define PIN_POT1 A1
#define PIN_POT2 A2
#define PIN_POT3 A3
#define PIN_POT4 A4
#define PIN_POT5 A5

#define EEPROM_SPOT_ADDRESS 0

LedLamp ledLamp = LedLamp();
LedControl<5> ledControl = LedControl<5>(PIN_PUSHBUTTON, true);

void setup()
{
  delay(300); // sanity delay
  ledControl.init();
  ledControl.setPotPin(0, PIN_POT1);
  ledControl.setPotPin(1, PIN_POT2);
  ledControl.setPotPin(2, PIN_POT3);
  ledControl.setPotPin(3, PIN_POT4);
  ledControl.setPotPin(4, PIN_POT5);


  ledLamp.init();
  ledLamp.loadFromEEPROM();
}

static CEveryNMillis controlSurfaceTime(50);

void loop()
{
  long startLoopTime = millis();

  uint8_t waitFor = ledLamp.render();
  while ((millis() - startLoopTime) < waitFor)
  {
    __asm__("nop\n\t");
  };

  if (controlSurfaceTime)
  {
    ledControl.updateValues();

    if (ledControl.hasNewPotValue(0))
    {
      ledLamp.setBrightness(ledControl.getPotValue(0));
    }
    if (ledControl.hasNewPotValue(1))
    {
      ledLamp.setParam1(ledControl.getPotValue(1));
    }
    if (ledControl.hasNewPotValue(2))
    {
      ledLamp.setParam2(ledControl.getPotValue(2));
    }
    if (ledControl.hasNewPotValue(3))
    {
      ledLamp.setParam3(ledControl.getPotValue(3));
    }
    if (ledControl.hasNewPotValue(4))
    {
      ledLamp.setParam4(ledControl.getPotValue(4));
    }

    if (ledControl.buttonClick()) {
      digitalWrite(13, LOW);
      ledLamp.nextMode();
    }

    if (ledControl.buttonClick1Second()) {
      ledLamp.saveToEEPROM();
    }
  }
}
