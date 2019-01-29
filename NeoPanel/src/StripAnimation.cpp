/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "stm32l4xx_hal.h"
#include "rng.h"
#include "StripAnimation.h"
#include "Strip.h"
#include "main.h"

StripAnimation::StripAnimation(Strip* strip) {
  m_strip = strip;
}


void StripAnimation::init()  {

  m_strip->setBright(.1);

  m_cpt = 0;
  m_state = STRIP_NOEL;
  m_stateTime = 0;
  m_transition = TRANSITION_SPARKLING_MULTICOLOR;
}


uint32_t StripAnimation::getRandomNumber(int max) {
  // uint32_t rnd = 0;
  // int hal_result = HAL_RNG_GenerateRandomNumber(&hrng, &rnd);
  // if (hal_result != HAL_OK) {
  //   return 4;
  // }
  // return rnd;

  return rand() % max;

}

void StripAnimation::oneSparkle(Color color, Color bgColor) {
  static int last_x = 0;
  static int last_y = 0;
  m_strip->setColor(last_x, last_y, bgColor);
  last_x = getRandomNumber(32);
  last_y = getRandomNumber(8);
  m_strip->setColor(last_x, last_y, color);
}

bool StripAnimation::scrollHorizontalLoop(const char* text, int loopNumber) {
  static int xScroll = 32;
  static int textLength ;
  static int loop;
  static int direction;
  static Color textColor1, textColor2;
  static Color bgColor = Color::black();
  if (m_stateTime == 0) {
    loop = 0;
    direction = -1;
    m_strip->setBright(.1f);
    xScroll = 32;
    textLength = 0;
    while (text[textLength ++] != 0);
    textColor1 = Color::randomColor();
    textColor2 = Color::randomColor();
  }

  m_strip->fillWith(bgColor);
  m_strip->print(xScroll, 0, text, textColor1, textColor2);
  oneSparkle(Color::randomColor(), bgColor);
  m_strip->flush();

  HAL_Delay(20);

  xScroll += direction;
  if (direction == -1 && xScroll == - (textLength * 6 - 26)) {
    direction = 1;
    loop ++;
  } else if (direction == 1 && xScroll == 12 && loop != loopNumber) {
    direction = -1;
  }


  return loop == loopNumber && xScroll > 35;

}

bool StripAnimation::scrollHorizontal(const char* text) {
  static int xScroll = 32;
  static int textLength ;
  static Color textColor1, textColor2;
  static Color bgColor = Color::black();
  if (m_stateTime == 0) {
      m_strip->setBright(.05f);
      xScroll = 32;
      textLength = 0;
      while (text[textLength ++] != 0);
      textColor1 = Color::randomColor();
      textColor2 = Color::randomColor();
  }

  m_strip->fillWith(bgColor);
  m_strip->print(xScroll, 0, text, textColor1, textColor2);
  oneSparkle(Color::randomColor(), bgColor);
  m_strip->flush();

  HAL_Delay(20);

  xScroll--;
  return xScroll < - (textLength - 1) * 6;
}

bool StripAnimation::scrollVertical(const char* text, Color textColor, Color bgColor) {
  static int yScroll = 32;
  static int yFreeze = 0;
  static int xPos = 0;
  if (m_stateTime == 0) {
      m_strip->setBright(.1f);
      yScroll = -10;
      yFreeze = 0;
      int textLength = 0;
      while (text[textLength ++] != 0);
      xPos = (32 - (textLength - 1) * 6) / 2;

  }

  m_strip->fillWith(bgColor);
  m_strip->print(xPos, yScroll, text, textColor, textColor);
  m_strip->flush();

  HAL_Delay(120);

  if (yScroll == 0) {
    if (yFreeze ++ == 10)  {
      yScroll ++;
    }
  } else { 
    yScroll++;
  }
  return yScroll > 10;
}

bool StripAnimation::noise() {
  if (m_stateTime == 0) {
    m_strip->setBright(1);
  }

  for (int c = 0; c < 32; c++) {
    for (int r = 0; r < 8; r++) { 
      float br = 0.0f;
      switch (getRandomNumber(4)) {
        case 0:
          br = 1.0f;
          break;
        case 1:
          br = 0.05;
          break;
      }
      m_strip->setColor(c, r, Color::randomColor().withBrightness(br));
    }
  }

  m_strip->flush();
  HAL_Delay(10);

  return m_stateTime > 200;
}

bool StripAnimation::rainbow() {
  static int xMiddle;
  static int xWidth;
  static float brRed, brGreen, brBlue;
  if (m_stateTime == 0) {
    m_strip->setBright(1);
    xWidth = 80;
    xMiddle = 32 + xWidth  / 2;
    brRed = 0.01f * (float)(getRandomNumber(100));
    brBlue = 0.01f * (float)(getRandomNumber(100));
    brGreen = 0.01f * (float)(getRandomNumber(100));
  }


  m_strip->fillWith(Color::black());
  for (int x = xMiddle - xWidth / 2; x < xMiddle + xWidth / 2; x++) {
//    Color color(0, 0,  );
    float br = 1.0f / ((float)abs(x - xMiddle) * 2  + 1.0f);
    Color color(br * brRed, br * brGreen, br * brBlue );
    for (int y = 0; y < 8 ; y++) {
      m_strip->setColor(x, y, color) ; 
    }
  }

  m_strip->flush();

  HAL_Delay(10);

  xMiddle --;
  return xMiddle < - xWidth ;
}


bool StripAnimation::tunnel() {
  static float tunnelPhase;
  static int tunnelCpt;
  static const float phaseInc = .1f;
  static Color tunnelColor1;
  static Color tunnelColor2;
  if (m_stateTime == 0) {
    m_strip->setBright(0.1f);

    tunnelPhase = 0.0f;
    tunnelCpt = 0;
    tunnelColor1 = Color::black();
    tunnelColor2 = Color::blue();
  }

  Color col = Color::fromTo(tunnelColor1, tunnelColor2, tunnelPhase);
  m_strip->drawRectangle(0, 0, 31, 7, col);
  m_strip->drawRectangle(1, 1, 2, 6, col);
  m_strip->drawRectangle(29, 1, 30, 6, col);

  col = Color::fromTo(tunnelColor1, tunnelColor2, tunnelPhase + phaseInc);
  m_strip->drawRectangle(3, 1, 28, 6, col);
  m_strip->drawRectangle(4, 2, 5, 5, col);
  m_strip->drawRectangle(26, 2, 27, 5, col);

  col = Color::fromTo(tunnelColor1, tunnelColor2, tunnelPhase + phaseInc * 2);
  m_strip->drawRectangle(6, 2, 25, 5, col);
  m_strip->drawRectangle(7, 3, 8, 4, col);
  m_strip->drawRectangle(23, 3, 24, 4, col);

  col = Color::fromTo(tunnelColor1, tunnelColor2, tunnelPhase + phaseInc * 3);
  m_strip->drawRectangle(9, 3, 22, 4, col);





  m_strip->flush();

  tunnelPhase += phaseInc;
  if (tunnelPhase > (1 - phaseInc)) {
    tunnelCpt ++;
    tunnelPhase = 0.0f;
    tunnelColor1 = tunnelColor2;
    switch (tunnelCpt) {
      case 1: 
        tunnelColor2 = Color::red();
        break;
      case 2: 
        tunnelColor2 = Color::blue();
        break;
      case 3: 
        tunnelColor2 = Color::yellow().withBrightness(.2f);
        break;
      case 4: 
        tunnelColor2 = Color::pink();
        break;
      case 5: 
        tunnelColor2 = Color::blue();
        break;
      default:
        tunnelColor2 = Color::randomColor();
        break;
    }
  }
  HAL_Delay(100);

  return tunnelCpt == 6;
}


bool StripAnimation::flash() {
  static int flashCounter;
  if (m_stateTime == 0) {
    m_strip->setBright(1.0);
    flashCounter = 0;
  }

  Color col(1.0f - 0.0039f * flashCounter, .0038f * flashCounter,  0);

  switch (flashCounter % 16) {
    case 0:
      m_strip->fillWith(col);
      break;
    default:
      m_strip->fillWith(Color::black());
      break;
  }

  m_strip->flush();

  HAL_Delay(10);
  flashCounter += 2;
  if (flashCounter > 255) {
    HAL_Delay(100);
  }
  return flashCounter > 255;
}




bool StripAnimation::sparkling(long delayTime, int min, int max) {
  static int last_x = 0;
  static int last_y = 0;
  static int last_x2 = 0;
  static int last_y2 = 0;
  static Color bgColor;
  if (m_stateTime == 0) {
      m_strip->setBright(1.0f);
      if (getRandomNumber(3) == 0) {
        bgColor = Color::randomColor().withBrightness(0.004f);
      } else {
        bgColor = Color::black();
      }
      m_strip->fillWith(bgColor);
  }

  m_strip->setColor(last_x, last_y, bgColor);
  m_strip->setColor(last_x2, last_y2, bgColor);
  last_x = getRandomNumber(32);
  last_y = getRandomNumber(8);
  last_x2 = getRandomNumber(32);
  last_y2 = getRandomNumber(8);
  Color col = Color::white();
  switch ((getRandomNumber(max-min)) + min) {
    case 0:
      col = Color::red();
      break;
    case 1:
      col = Color::green();
      break;
    case 2:
      col = Color::blue();
      break;
    case 3:
      col = Color(0, 1, 1);
      break;
    case 4:
      col = Color(1, 0, 1);
      break;
    case 5:
      col = Color( 1, 1, 0);
      break;
    default:
      break;
  }
  m_strip->setColor(last_x, last_y, col);
  m_strip->setColor(last_x2, last_y2, col);

  m_strip->flush();

  HAL_Delay(delayTime);

  return m_stateTime > 200;
}



void StripAnimation::loop() {

  // Display Mghz !!
  // char mghz[12];
  // bool space = true;
  // mghz[11] = 0;
  // for (int i = 0; i <= 10; i++) {
  //   int p = 1;
  //   for (int k = 0; k < (10 - i); k++) {
  //     p *= 10;
  //   }
  //   int d = (SystemCoreClock / p) % 10;
  //   if (d == 0 && space) {
  //     mghz[i] = ' ';
  //   } else {
  //     mghz[i] = (char) 48 + d;
  //     space = false;
  //   }
  // }

  // //        nextState = scrollHorizontal(mghz);


  bool nextState = false;
  switch (m_state) {
    case STRIP_SPARKLING_1:
    case STRIP_SPARKLING_2:
    case STRIP_SPARKLING_3:
    case STRIP_SPARKLING_4:
    case STRIP_SPARKLING_5:
    case STRIP_SPARKLING_6:
    case STRIP_SPARKLING_7:
    case STRIP_SPARKLING_8:
    case STRIP_SPARKLING_9:
    case STRIP_SPARKLING_10:
      switch (m_transition) {
        case TRANSITION_SPARKLING_MULTICOLOR:
          nextState = sparkling(15, 0, 7);
          break;
        case TRANSITION_SPARKLING_RED:
          nextState = sparkling(15, 0, 0);
          break;
        case TRANSITION_SPARKLING_WHITE:
          nextState = sparkling(15, 7,7);
          break;
        case TRANSITION_SPARKLING_GREEN:
          nextState = sparkling(15, 1,1);
          break;
        case TRANSITION_RAINBOW:
          nextState = rainbow();
          break;
        case TRANSITION_TUNNEL:
          nextState = tunnel();
          break;
      }
      break;
    case STRIP_NOEL:
      nextState = scrollVertical("NOEL", Color::red(), Color::green().withBrightness(.1f));
      break;
    case STRIP_2018:
      nextState = scrollVertical("2018", Color::red(), Color::green().withBrightness(.1f));
      break;
    case STRIP_HOSXE_XP:
        nextState = scrollHorizontal("Bienvenue a Thais, Mael, Alex et Nico");
        break;
    case STRIP_BIENVENUE:
      nextState = scrollHorizontal("Le pere noel vous surveille !");
      break;
    case STRIP_HOSXE_GP:
      nextState = scrollHorizontal("Concours de Floss a 18h");
      break;
    case STRIP_LAURENT:
      nextState = scrollHorizontal("Thais aime trop les lego friends");
      break;
    case STRIP_MATHIAS:
      nextState = scrollHorizontal("Mael est trop fort");
      break;
    case STRIP_CAROLE:
      nextState = scrollHorizontal("Hmmmmmm.... les bonnes tagliatelles au saumon");
      break;
    case STRIP_MARION:
      nextState = scrollHorizontal("23 décembre 2018 : Gouter avec les Seguy");
      break;
    case STRIP_JOYEUX_NOEL: 
      nextState = scrollHorizontalLoop("Joyeux noel 2018", 2);
      break;
    case STRIP_FINISHED:
      // m_state ++ in next State bellow
      m_state = -1;
      nextState = true;
      break;
    case STRIP_BLUETOOTH_COMMAND: {
      // We don't set nextState for flash info 
      if (m_stateTimeFlashInfo == 0) {
        m_strip->fillWith(Color::black());
        m_stateTime = 0;
      }
      bool localNextStep = false;
      switch (m_blueToothState) {
        case BLUETOOTH_FLASHINFO:
          localNextStep = scrollHorizontalLoop(m_flashInfoText, 3);
          break;
        case BLUETOOTH_FLASH:
          localNextStep = flash();
          break;
        case BLUETOOTH_NOISE:
          localNextStep = noise();
          break;
        case BLUETOOTH_TUNNEL:
          localNextStep = tunnel();
          break;
      }
      if (localNextStep) {
        m_state = m_stateBeforeFlash;
        m_stateTime = 0;
        m_strip->fillWith(Color::black());
      }
      // different from 0 is enough
      m_stateTimeFlashInfo = 1;
      break;
    }
    default:
      nextState = scrollVertical("ERROR", Color::red(), Color::black()); 
      break;
  }

  if (nextState) {
    m_state++;
    m_stateTime = 0;
    m_strip->fillWith(Color::black());
    m_transition = getRandomNumber(TRANSITION_LAST);
  } else {
    m_stateTime++;
  }

}


void StripAnimation::loop2() {

  // Display Mghz !!
  // char mghz[12];
  // bool space = true;
  // mghz[11] = 0;
  // for (int i = 0; i <= 10; i++) {
  //   int p = 1;
  //   for (int k = 0; k < (10 - i); k++) {
  //     p *= 10;
  //   }
  //   int d = (SystemCoreClock / p) % 10;
  //   if (d == 0 && space) {
  //     mghz[i] = ' ';
  //   } else {
  //     mghz[i] = (char) 48 + d;
  //     space = false;
  //   }
  // }

  // //        nextState = scrollHorizontal(mghz);


  bool nextState = false;
  switch (m_state) {
    case STRIP_SPARKLING_1:
    case STRIP_SPARKLING_2:
    case STRIP_SPARKLING_3:
    case STRIP_SPARKLING_4:
    case STRIP_SPARKLING_5:
    case STRIP_SPARKLING_6:
    case STRIP_SPARKLING_7:
    case STRIP_SPARKLING_8:
    case STRIP_SPARKLING_9:
    case STRIP_SPARKLING_10:
      switch (m_transition) {
        case TRANSITION_SPARKLING_MULTICOLOR:
          nextState = sparkling(15, 0, 7);
          break;
        case TRANSITION_SPARKLING_RED:
          nextState = sparkling(15, 0, 0);
          break;
        case TRANSITION_SPARKLING_WHITE:
          nextState = sparkling(15, 7,7);
          break;
        case TRANSITION_SPARKLING_GREEN:
          nextState = sparkling(15, 1,1);
          break;
        case TRANSITION_RAINBOW:
          nextState = rainbow();
          break;
        case TRANSITION_TUNNEL:
          nextState = tunnel();
          break;
      }
      break;
    case STRIP_NOEL:
      nextState = scrollVertical("ANNEE", Color::red(), Color::green().withBrightness(.1f));
      break;
    case STRIP_2018:
      nextState = scrollVertical("2019", Color::red(), Color::green().withBrightness(.1f));
      break;
    case STRIP_HOSXE_XP:
        nextState = scrollHorizontal("Jeremie et Timothee sont trop forts !");
        break;
    case STRIP_BIENVENUE:
      nextState = scrollHorizontal("Bienvenue aux Leyre chez Nous");
      break;
    case STRIP_HOSXE_GP:
      nextState = scrollHorizontal("Vive Juliette et Capucine !");
      break;
    case STRIP_LAURENT:
      nextState = scrollHorizontal("Paris + Londres = LOVE");
      break;
    case STRIP_MATHIAS:
      nextState = scrollHorizontal("Dejeuner dominical du 6 Janvier");
      break;
    case STRIP_CAROLE:
      nextState = scrollHorizontal("Le pere noel vous surveille");
      break;
    case STRIP_MARION:
      nextState = scrollHorizontal("Attention chat d'anges heureux");
      break;
    case STRIP_JOYEUX_NOEL: 
      nextState = scrollHorizontalLoop("Bonne Annee 2019", 2);
      break;
    case STRIP_FINISHED:
      // m_state ++ in next State bellow
      m_state = -1;
      nextState = true;
      break;
    case STRIP_BLUETOOTH_COMMAND: {
      // We don't set nextState for flash info 
      if (m_stateTimeFlashInfo == 0) {
        m_strip->fillWith(Color::black());
        m_stateTime = 0;
      }
      bool localNextStep = false;
      switch (m_blueToothState) {
        case BLUETOOTH_FLASHINFO:
          localNextStep = scrollHorizontalLoop(m_flashInfoText, 3);
          break;
        case BLUETOOTH_FLASH:
          localNextStep = flash();
          break;
        case BLUETOOTH_NOISE:
          localNextStep = noise();
          break;
        case BLUETOOTH_TUNNEL:
          localNextStep = tunnel();
          break;
      }
      if (localNextStep) {
        m_state = m_stateBeforeFlash;
        m_stateTime = 0;
        m_strip->fillWith(Color::black());
      }
      // different from 0 is enough
      m_stateTimeFlashInfo = 1;
      break;
    }
    default:
      nextState = scrollVertical("ERROR", Color::red(), Color::black()); 
      break;
  }

  if (nextState) {
    m_state++;
    m_stateTime = 0;
    m_strip->fillWith(Color::black());
    m_transition = getRandomNumber(TRANSITION_LAST);
  } else {
    m_stateTime++;
  }

}


  // Display Mghz !!
  // char mghz[12];
  // bool space = true;
  // mghz[11] = 0;
  // for (int i = 0; i <= 10; i++) {
  //   int p = 1;
  //   for (int k = 0; k < (10 - i); k++) {
  //     p *= 10;
  //   }
  //   int d = (SystemCoreClock / p) % 10;
  //   if (d == 0 && space) {
  //     mghz[i] = ' ';
  //   } else {
  //     mghz[i] = (char) 48 + d;
  //     space = false;
  //   }
  // }

 
void StripAnimation::setFlashInfo(char* flashInfoText) {
  if (strcmp(flashInfoText, ":Reset") == 0) {  
    m_strip->setBright(.1);
    m_cpt = 0;
    m_state = STRIP_NOEL;
    m_stateTime = 0;
    m_transition = TRANSITION_SPARKLING_MULTICOLOR;
    return;
  }  else if (strcmp(flashInfoText, ":Bright") == 0) {  
    m_strip->setBright(1.00);
    return;
  }

  // Don't save FLASH_INFO mode as previous mode
  if (m_state != STRIP_BLUETOOTH_COMMAND) {
    m_stateBeforeFlash = m_state;
  }

  if (strcmp(flashInfoText, ":Tunnel") == 0) {
    m_blueToothState = BLUETOOTH_TUNNEL;
  } else if (strcmp(flashInfoText, ":Flash") == 0) {
    m_blueToothState = BLUETOOTH_FLASH;
  } else if (strcmp(flashInfoText, ":Noise") == 0) {
    m_blueToothState = BLUETOOTH_NOISE;
  } else {
    strcpy(m_flashInfoText, flashInfoText);
    m_blueToothState = BLUETOOTH_FLASHINFO;
  }

  m_state = STRIP_BLUETOOTH_COMMAND;
  m_stateTimeFlashInfo = 0;
}
