/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/

#include "LedSpot.h"
#include <EEPROM.h>

LedSpot::LedSpot(uint16_t numberOfPixel, uint8_t pin)
{
    // Declare output pin
    pinMode(pin, OUTPUT);

    m_leds = new Adafruit_NeoPixel(numberOfPixel, pin, NEO_GRB + NEO_KHZ800);
    m_brightness = 127;
    m_numberOfPixels = numberOfPixel;
    m_leds->begin();

    m_animationStep = 0;
    m_animationType = ANIM_1;
    m_animationSpeed = 127;
    m_animationSubStep = 0;

    m_lastUpdatedLed = 0;
}

void LedSpot::setAllColor(uint32_t c)
{
    for (uint16_t i = 0; i < m_numberOfPixels; i++)
    {
        m_leds->setPixelColor(i, c);
    }
}

void LedSpot::setBrightness(uint8_t brightness)
{
    if (brightness < 1)
    {
        brightness = 1;
    }
    m_brightness = brightness;
    m_leds->setBrightness(m_brightness);
}

void LedSpot::show()
{
    m_leds->show();
}

uint32_t LedSpot::getColorFrom8bitsValue(uint8_t value, bool stopAtwhite)
{
    static uint8_t colors[8] = {
        0b000,
        0b001,
        0b101,
        0b100,
        0b110,
        0b010,
        0b011,
        0b111};

    // Use 3 MSBits for main color
    // Use other 4 for interpolation
    uint8_t rgb1 = value >> 5;
    uint8_t rgb2;

    if (rgb1 == 0)
    {
        rgb1 = 1;
        rgb2 = 1;
    }
    else
    {
        if (stopAtwhite)
        {
            rgb2 = rgb1 < 7 ? rgb1 + 1 : 7; // remains on white at the end
        }
        else
        {
            rgb2 = rgb1 < 7 ? rgb1 + 1 : 1;
        }
    }
    // translate to nicer colors
    rgb1 = colors[rgb1];
    rgb2 = colors[rgb2];

    uint8_t interp = value & 0x1f;

    // Then compute RGB components
    uint8_t red = ((rgb1 >> 2) * (0x1f - interp) + (rgb2 >> 2) * interp) << 3;
    uint8_t green = (((rgb1 >> 1) & 0x1) * (0x1f - interp) + ((rgb2 >> 1) & 0x1) * interp) << 3;
    uint8_t blue = ((rgb1 & 0x1) * (0x1f - interp) + (rgb2 & 0x1) * interp) << 3;

    return Adafruit_NeoPixel::Color(red, green, blue);
}

void LedSpot::incLastUpdatedLed()
{
    m_lastUpdatedLed++;
    if (m_lastUpdatedLed >= m_numberOfPixels)
    {
        m_lastUpdatedLed = 0;
    }
}

uint8_t LedSpot::getLastUpdatedOpositeLed()
{
    if (m_numberOfPixels == 7)
    {
        return ((m_lastUpdatedLed - 1 + (m_numberOfPixels - 1) / 2) % (m_numberOfPixels - 1)) + 1;
    }
    else
    {
        return (m_lastUpdatedLed + m_numberOfPixels / 2) % m_numberOfPixels;
    }
}

void LedSpot::animationStart(LedAnimation anim)
{
    m_animationType = anim;
    m_animationStep = 1;
}

void LedSpot::animationNextStep()
{

    switch (m_animationType)
    {
    case ANIM_0:
        setAllColor(getColorFrom8bitsValue(m_animationSpeed, true));
        break;
    case ANIM_1:
    {
        uint8_t numberOfSteps;
        m_animationSubStep += (m_animationSpeed << 2) - 8;
        if (m_animationSubStep < -8)
        {
            m_animationSubStep = 0;
        }
        numberOfSteps = m_animationSubStep / 0xff;
        if (numberOfSteps > 0)
        {
            m_animationSubStep = m_animationSubStep % 0xff;
            m_animationStep += numberOfSteps;
            if (m_animationStep > 0xff)
            {
                m_animationStep = 0x20 + (0xff - m_animationStep);
            }
        }
        setAllColor(getColorFrom8bitsValue(m_animationStep, false));
    }
    break;
    case ANIM_2:
    {
        uint8_t numberOfSteps;
        m_animationSubStep += m_animationSpeed;
        numberOfSteps = m_animationSubStep / 0xff;
        if (numberOfSteps > 0)
        {
            m_animationSubStep = m_animationSubStep % 0xff;
            m_animationStep += numberOfSteps;
            if (m_animationStep > 0xff)
            {
                m_animationStep = 0x20 + (0xff - m_animationStep);
            }
        }

        uint32_t animColor = getColorFrom8bitsValue(m_animationStep, false);
        uint32_t animColor2 = getColorFrom8bitsValue(((m_animationStep + 16) % 0xff), false);
        m_leds->setPixelColor(m_lastUpdatedLed, Adafruit_NeoPixel::Color(0, 0, 0));
        m_leds->setPixelColor(getLastUpdatedOpositeLed(), Adafruit_NeoPixel::Color(0, 0, 0));

        if (numberOfSteps > 0)
        {
            incLastUpdatedLed();
            // if 7 leds go back to 1 (not 0)
            if (m_numberOfPixels == 7 && m_lastUpdatedLed == 0)
            {
                m_lastUpdatedLed = 1;
            }
        }

        m_leds->setPixelColor(m_lastUpdatedLed, animColor);
        m_leds->setPixelColor(getLastUpdatedOpositeLed(), animColor2);
    }
    break;
    case ANIM_4:
        m_animationStep++;
        m_animationSubStep++;
        if (m_animationStep > (16 - (m_animationSpeed >> 4)))
        {
            m_animationStep = 0;
            setAllColor(getColorFrom8bitsValue(m_animationSubStep % 0xff, false));
        }
        else
        {
            setAllColor(Adafruit_NeoPixel::Color(0, 0, 0));
        }
        break;
    case ANIM_3:
    {
        static int lastPixel1 = 0, lastPixel2 = 0, lastPixel3 = 0;
        m_animationStep++;
        m_leds->setPixelColor(lastPixel1, Adafruit_NeoPixel::Color(0, 0, 0));
        m_leds->setPixelColor(lastPixel2, Adafruit_NeoPixel::Color(0, 0, 0));
        m_leds->setPixelColor(lastPixel3, Adafruit_NeoPixel::Color(0, 0, 0));
        if (m_animationStep > (8 - (m_animationSpeed >> 5)))
        {
            m_animationStep = 0;
            lastPixel1 = random(m_numberOfPixels);
            m_leds->setPixelColor(lastPixel1, getRandomColor());
            lastPixel2 = random(m_numberOfPixels);
            m_leds->setPixelColor(lastPixel2, getRandomColor());
            lastPixel3 = random(m_numberOfPixels);
            m_leds->setPixelColor(lastPixel3, getRandomColor());
        }
    }
    break;
    default:
        // ERROR !
        setAllColor(Adafruit_NeoPixel::Color(200, 0, 0));
        break;
    }
}

void LedSpot::setAnimationSpeed(uint8_t animationSpeed)
{
    m_animationSpeed = animationSpeed;
}

void LedSpot::nextAnimation()
{
    m_animationType = static_cast<LedAnimation>(m_animationType + 1);

    if (m_animationType >= ANIM_FINISHED)
    {
        m_animationType = ANIM_0;
        m_animationStep = 0;
        m_animationSubStep = 0;
    }
    setAllColor(Adafruit_NeoPixel::Color(0, 0, 0));
}

uint32_t LedSpot::getRandomColor()
{
    switch (random(6))
    {
    case 0:
        return Adafruit_NeoPixel::Color(random(255), random(255), random(255));
        break;
    case 1:
        return Adafruit_NeoPixel::Color(255, 0, 0);
        break;
    case 2:
        return Adafruit_NeoPixel::Color(255, 255, 0);
        break;
    case 3:
        return Adafruit_NeoPixel::Color(255, 0, 255);
        break;
    case 4:
        return Adafruit_NeoPixel::Color(0, 0, 255);
        break;
    case 5:
        return Adafruit_NeoPixel::Color(0, 255, 255);
        break;
    }
    return Adafruit_NeoPixel::Color(255, 255, 255);
}

void LedSpot::loadStateFromEEPROM(int index)
{
    m_animationType = EEPROM.read(index);
    m_animationSpeed = EEPROM.read(index + 1);
    m_animationStep = EEPROM.read(index + 2);
    m_animationStep += EEPROM.read(index + 3) << 8;
    setBrightness(EEPROM.read(index + 4));
}

void LedSpot::saveStateToEEPROM(int index)
{
    EEPROM.write(index, m_animationType);
    EEPROM.write(index + 1, m_animationSpeed);
    EEPROM.write(index + 2, m_animationStep & 0xff);
    EEPROM.write(index + 3, m_animationStep >> 8);
    EEPROM.write(index + 4, m_brightness);
}

void LedSpot::acknowledge(uint32_t color)
{
    m_leds->setBrightness(255);
    for (int k = 0; k < 5; k++)
    {
        setAllColor(Adafruit_NeoPixel::Color(0, 0, 0));
        m_leds->show();
        delay(50);
        setAllColor(color);
        m_leds->show();
        delay(20);
    }
    setAllColor(Adafruit_NeoPixel::Color(0, 0, 0));
    m_leds->show();
    delay(100);
    m_leds->setBrightness(m_brightness);
}