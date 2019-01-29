/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/


#ifndef STRIP_ANIMATION_H
#define STRIP_ANIMATION_H


enum  STRIP_STATE {
  STRIP_JOYEUX_NOEL = 0,
  STRIP_SPARKLING_1,
  STRIP_BIENVENUE,
  STRIP_SPARKLING_2,
  STRIP_LAURENT,
  STRIP_SPARKLING_3,
  STRIP_HOSXE_GP,
  STRIP_SPARKLING_4,
  STRIP_MARION,
  STRIP_SPARKLING_5,
  STRIP_CAROLE,
  STRIP_SPARKLING_6,
  STRIP_MATHIAS,
  STRIP_SPARKLING_7,
  STRIP_HOSXE_XP,
  STRIP_SPARKLING_8,
  STRIP_NOEL,
  STRIP_2018,
  STRIP_SPARKLING_9,
  STRIP_SPARKLING_10,
  STRIP_FINISHED,
  STRIP_BLUETOOTH_COMMAND,
};

enum BLUETOOTH_COMMAND {
    BLUETOOTH_FLASHINFO,
    BLUETOOTH_FLASH,
    BLUETOOTH_NOISE,
    BLUETOOTH_TUNNEL
};


enum TRANSITION_TYPE {
    TRANSITION_SPARKLING_MULTICOLOR = 0,
    TRANSITION_RAINBOW,
    TRANSITION_SPARKLING_RED,
    TRANSITION_SPARKLING_WHITE,
    TRANSITION_SPARKLING_GREEN,
    TRANSITION_LAST,
    TRANSITION_TUNNEL
};


class Strip;
class Color;

class StripAnimation {
    public:
        StripAnimation(Strip* strip);
        void init();
        void loop();
        void loop2();
        void oneSparkle(Color color, Color bgColor);
        bool scrollHorizontalLoop(const char* text, int loopNumber);
        bool scrollHorizontal(const char* text);
        bool scrollVertical(const char* text, Color textColor, Color bgColor);
        bool rainbow();
        bool flash();
        bool noise();
        bool tunnel();
        bool sparkling(long delayTime, int min, int max);
        uint32_t getRandomNumber(int max);
        void setFlashInfo(char* flashInfoText);

    private:
        Strip *m_strip;
        int m_cpt ;
        int m_stateTime ;
        int m_stateTimeFlashInfo ;
        int m_state;
        int m_transition;
        int abs(int i) { return i < 0 ? -i : i; }
        char m_flashInfoText[64 + 1];
        int m_stateBeforeFlash;
        int m_blueToothState;
};


#endif