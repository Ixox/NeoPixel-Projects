/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/


#include "Led.h"
#include "stm32l4xx_hal.h"


static inline void delay_ns(uint32_t ns) {
    // 80 Mghz
    ns *= 80;
    ns /= 3000;

    /* fudge for function call overhead  */
    //us--;
    asm volatile("   mov r0, %[ns]          \n\t"
                 "1: subs r0, #1            \n\t"
                 "   bhi 1b                 \n\t"
                 :
                 : [ns] "r" (ns)
                 : "r0");
};

Led::Led() {
    this->m_color.r = 0;
    this->m_color.g = 0;
    this->m_color.b = 0;
}

void Led::flush(GPIO_TypeDef* ledGpio, int ledPin, float bright) {
    sendBits(ledGpio, ledPin, this->m_color.g * bright * 255.0f);
    sendBits(ledGpio, ledPin, this->m_color.r * bright * 255.0f);
    sendBits(ledGpio, ledPin, this->m_color.b * bright * 255.0f);
}

void Led::setColor(float r, float g, float b) {
    this->m_color.r = r;
    this->m_color.g = g;
    this->m_color.b = b;
}
void Led::setColor(Color color) {
    this->m_color.r = color.r * color.brightness;
    this->m_color.g = color.g * color.brightness;
    this->m_color.b = color.b * color.brightness;
}
void Led::addColor(float r, float g, float b) {
    this->m_color.r += r;
    this->m_color.g += g;
    this->m_color.b += b;
}

void Led::sendBits(GPIO_TypeDef *ledGpio, int ledPin, int c) {
    c = c > 255 ? 255 : c;
    for (int i = 0; i < 8; i++) { 
        int bit = ((c >> (7 - i)) & 0x1);
        if (bit == 0) {
            HAL_GPIO_WritePin(ledGpio, ledPin, GPIO_PIN_SET);
            delay_ns(50);
            HAL_GPIO_WritePin(ledGpio, ledPin, GPIO_PIN_RESET);
            delay_ns(1100);
        } else {
            HAL_GPIO_WritePin(ledGpio, ledPin, GPIO_PIN_SET);
            delay_ns(900);
            HAL_GPIO_WritePin(ledGpio, ledPin, GPIO_PIN_RESET);
            delay_ns(300);
        }
    }
}

