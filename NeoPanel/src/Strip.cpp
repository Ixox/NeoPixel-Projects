/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/


#include "Strip.h"
#include "font_6x8.h"



Strip::Strip(GPIO_TypeDef* ledGpio, int ledPin) {
    this->ledGpio = ledGpio;
    this->ledPin = ledPin;
    this->bright = 0.5;
}

void Strip::setColor(int x , int y, float r, float g, float b) {
    if (x >= 0 && x <= 31 && y >= 0 && y <= 7) {
        leds[x][y].setColor(r, g, b);
    }
}

void Strip::setColor(int x , int y, Color color) {
    if (x >= 0 && x <= 31 && y >= 0 && y <= 7) {
        leds[x][y].setColor(color);
    }
}

void Strip::addColor(int x , int y, float r, float g, float b) {
    if (x >= 0 && x <= 31 && y >= 0 && y <= 7) {
        leds[x][y].addColor(r, g, b);
    }
}

void Strip::setBright(float b) {
    this->bright = b;
}

void Strip::flush() {
    for (int c = 31; c >= 0 ; c--) {
        if (c & 1) {
            for (int r = 7; r >= 0 ; r--) {
                leds[c][r].flush(this->ledGpio, this->ledPin, this->bright);
            }
        } else {
            for (int r = 0; r < 8 ; r++) {
                leds[c][r].flush(this->ledGpio, this->ledPin, this->bright);
            }
        }
    }
}


void Strip::print(int x, int y, const char *text, Color textcolor1, Color textcolor2) {
    Color colors[32];
    for (int c = 0; c < 32; c++) {
        float cf1 = 1.0f * c / 31.0f ;
        float cf2 = 1 - cf1;
        colors[c] = Color(  textcolor1.r * cf1 + textcolor2.r * cf2,
                            textcolor1.g * cf1 + textcolor2.g * cf2,
                            textcolor1.b * cf1 + textcolor2.b * cf2);
    }
    for (int t = 0; text[t] != 0; t++) {
        for (int row = 0; row < 8 ; row++) {
            char bits = console_font_6x8[text[t] * 8 + row];
            for (int col = 0; col < 5 ; col++) {
                char bit = (bits >> (6 - col)) & 0x1; 
                if (bit > 0) {
                    int xOnStrip = t * 6 + col + x;
                    int yOnStrip = row + y;
                    // setColor test if x and y are OK
                    if (xOnStrip >= 0 && xOnStrip <=31) {
                        setColor(xOnStrip, yOnStrip, colors[xOnStrip]); 
                    }
                }
            }
        }
    }
}


void Strip::fillWith(Color bgColor) {
    for (int row = 0; row < 8 ; row++) {
        for (int col = 0; col < 32 ; col++) {
            setColor(col, row, bgColor); 
        }
    }
}


bool Strip::drawRectangle(int minX, int minY, int maxX, int maxY, Color col) {

  for (int x = minX; x <= maxX ; x++) {
    setColor(x, minY, col);
    setColor(x, maxY, col);
  }

  for (int y = minY; y <= maxY ; y++) {
    setColor(minX, y, col);
    setColor(maxX, y, col);
  }

}
