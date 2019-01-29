/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/


#ifndef COLOR_H
#define COLOR_H


class Color {
    friend class Led;
    friend class Strip;
    public:
        Color();
        Color(float red, float green, float blue);
        Color(float red, float green, float blue, float brightness);
        static Color &black(); 
        static Color &red();
        static Color &green();
        static Color &blue();
        static Color &white();
        static Color &yellow();
        static Color &pink();
        static Color &turquoise();
        static Color &randomColor();
        Color withBrightness(float br);
        float getRed() { return r;}
        float getGreen() { return g;}
        float getBlue() { return b;}
        float getBrightness() { return brightness; }
        static Color &fromTo(Color &col1, Color &col2, float value);
        
    private:
        // , blue, red, green, yellow, white, pink;
        float r, g, b;
        float brightness;
};


#endif