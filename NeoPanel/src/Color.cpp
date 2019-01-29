/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/


#include <stdlib.h>
#include "Color.h"
#include "rng.h"

Color::Color() {
    this->r = 0;
    this->g = 0;
    this->b = 0;
    this->brightness = 1.0f;
}
Color::Color(float red, float green, float blue) {
    this->r = red;
    this->g = green;
    this->b = blue;
    this->brightness = 1.0f;
}
Color::Color(float red, float green, float blue, float brightness) {
    this->r = red;
    this->g = green;
    this->b = blue;
    this->brightness = brightness;
}



Color &Color::fromTo(Color &col1, Color &col2, float value) {
    static Color fromToColor(0,0,0);
    value = value > 1.0f ? 1.0f : value;
    fromToColor.r = col1.r * (1.0f - value) + col2.r * value;
    fromToColor.g = col1.g * (1.0f - value) + col2.g * value;
    fromToColor.b = col1.b * (1.0f - value) + col2.b * value;
    fromToColor.brightness = col1.brightness * (1.0f - value) + col2.brightness * value;
    return fromToColor;
}


Color &Color::black() { static Color color(0,0,0); return color; }
Color &Color::red() { static Color color(1,0,0); return color; }
Color &Color::green() { static Color color(0,1,0); return color; }
Color &Color::blue() { static Color color(0,0,1); return color; }
Color &Color::white() { static Color color(1,1,1); return color; }
Color &Color::yellow() { static Color color(1,1,0); return color; }
Color &Color::pink() { static Color color(1,0,1); return color; }
Color &Color::turquoise() { static Color color(0,1,1); return color; }

Color &Color::randomColor() {
    switch (rand() % 7) {
        case 0: 
            return red();
        case 1: 
            return green();
        case 2: 
            return blue();
        case 3: 
            return yellow();
        case 4: 
            return pink();
        case 5: 
            return turquoise();
        case 6: 
            return white();
    }
    return black();
}

Color Color::withBrightness(float br) {
    return Color(this->r, this->g, this->b, br);
}