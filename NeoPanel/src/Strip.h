/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/


#ifndef STRIP_H
#define STRIP_H

#include "stm32l4xx_hal.h"
#include "Led.h"

class Strip {
    public:
        Strip(GPIO_TypeDef* ledGpio,  int ledPin);
        void setColor(int x , int y, float r, float g, float b);
        void setColor(int x , int y, Color color);
        void addColor(int x , int y, float r, float g, float b);
        void setBright(float b);
        void flush();

        void print(int x, int y, const char *text, Color textColor1, Color textcolor2);
        void fillWith(Color bgColor);

        bool drawRectangle(int minX, int minY, int maxX, int maxY, Color color);
    private:
        Led leds[32][8];
        float bright;
        int ledPin;
        GPIO_TypeDef* ledGpio;

};


#endif