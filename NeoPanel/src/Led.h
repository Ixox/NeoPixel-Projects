/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/


#ifndef LED_H
#define LED_H

#include "Color.h"
#include "stm32l4xx_hal.h"

class Led  {
    public:
        Led();
        void flush(GPIO_TypeDef* ledGpio, int ledPin, float bright);
        void setColor(float r, float g, float b);
        void setColor(Color color);
        void addColor(float r, float g, float b);
    private:
        Color m_color;
        void sendBits(GPIO_TypeDef *ledGpio, int ledPin, int c);
};


#endif