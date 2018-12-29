/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/

#include "LedSpot.h"
#include "LedControl.h"


// Neopixel D2
#define PIN_NEOPIXEL A5
// Pushbutton D3
#define PIN_PUSHBUTTON A4
// analog input
#define PIN_POT A0

#define NUMPIXELS 12

#define EEPROM_SPOT_ADDRESS 0

// initialize led spot !
LedSpot ledSpot = LedSpot(NUMPIXELS, PIN_NEOPIXEL);
LedControl ledControl = LedControl(PIN_PUSHBUTTON, PIN_POT);

/* 
  Initialize pin mode
*/
void setup()
{
    // Pin Mode for Pot (GND and 5V) and button GND
    pinMode(A1, OUTPUT);
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    digitalWrite(A1, LOW);
    digitalWrite(A2, HIGH);
    digitalWrite(A3, LOW);

    // START !
    ledSpot.loadStateFromEEPROM(EEPROM_SPOT_ADDRESS);
    ledSpot.acknowledge(Adafruit_NeoPixel::Color(0, 150, 0));

    ledControl.init();
}

bool memorySaved = false;

void loop()
{
    unsigned long time = millis();

    ledControl.updateValues();

    // animation
    ledSpot.animationNextStep();
    if (ledControl.hasNewPotValue())
    {
        ledSpot.setAnimationSpeed(ledControl.getPotValue());
    }

    // Set Brightness if button pushed for more than 1 sec
    if (ledControl.hasNewPotValueButtonPushed())
    {
        ledSpot.setBrightness(ledControl.getPotValue());
    }

    if (ledControl.buttonClick())
    {
        ledSpot.nextAnimation();
        ledControl.reinitPotThreshold();
    }

    if (ledControl.buttonClick1Second() && !memorySaved)
    {
        ledSpot.saveStateToEEPROM(EEPROM_SPOT_ADDRESS);
        ledSpot.acknowledge(Adafruit_NeoPixel::Color(150, 150, 0));
        ledControl.reinitPotThreshold();
    }

    ledSpot.show();
    
    // Wait for 20 ms
    while (millis() < (time + 20))
        ;
}
