/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/

#include "EEPROM.h"
#include "LedLamp.h"

//const uint16_t energymap[8] PROGMEM = {10, 70, 140, 190, 210, 225, 240, 255};
//const uint8_t energymap[8] PROGMEM = {0, 0, 00, 0, 0, 0, 0, 0};
const uint8_t energymap[8] PROGMEM = {10, 70, 140, 190, 210, 225, 240, 255};

const CRGBPalette16 palettes[] PROGMEM = {
    RainbowColors_p,
    RainbowStripeColors_p,
    OceanColors_p,
    CloudColors_p,
    ForestColors_p,
    PartyColors_p,
    HeatColors_p,
    LavaColors_p,
 };

// torch mode
// ==========
enum
{
  torch_passive = 0,    // just environment, glow from nearby radiation
  torch_nop = 1,        // no processing
  torch_spark = 2,      // slowly looses energy, moves up
  torch_spark_temp = 3, // a spark still getting energy from the level below
};

void LedLamp::init()
{
  m_mode = MODE_STATIC;
  m_brightness = 255;

  FastLED.addLeds<WS2812B, NEOPIXEL_PIN, COLOR_ORDER>(leds, NUMBER_OF_LEDS);
  //  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setCorrection(UncorrectedColor);
  FastLED.setBrightness(m_brightness);
  //FastLED.setDither(false);
  FastLED.setDither(false);

  for (int i = 0; i < NUMBER_OF_LEDS_FIRE; i++)
  {
    m_param[i].currentEnergy = 0;
    m_param[i].nextEnergy = 0;
    m_param[i].energyMode = torch_passive;
  }

  for (int i = 0; i < NUMBER_OF_LEDS; i++)
  {
    leds[i] = CRGB(0, 0, 30);
  }


  // init noise 
    // Initialize our coordinates to some random values
  noisex = random16();
  noisey = random16();
  noisez = random16();
  m_currentPalette = palettes[0];
  m_param1 = 96;
  m_param2 = 127;
  m_param3 = 140;
  m_param4 = 0;
}

void LedLamp::saveToEEPROM() {
  EEPROM.write(0, m_mode);
  EEPROM.write(1, m_param1);
  EEPROM.write(2, m_param2);
  EEPROM.write(3, m_param3);
  EEPROM.write(4, m_param4);
  EEPROM.write(5, m_brightness);

  for (int i = 0; i < 5; i++) {
    for (int k=0; k < NUMBER_OF_LEDS; k++) {
      leds[k] = CRGB(0, 0, 0);
    }
    FastLED.show();
    delay(100);
    for (int k=0; k < NUMBER_OF_LEDS; k++) {
      leds[k] = CRGB(0, 0, 30);
    }
    FastLED.show();
    delay(30);
  }
}
void LedLamp::loadFromEEPROM() {
  m_mode = EEPROM.read(0);
  if (m_mode < MODE_LAST) {
    setParam1(EEPROM.read(1));
    setParam2(EEPROM.read(2));
    setParam3(EEPROM.read(3));
    setParam4(EEPROM.read(4));
    setBrightness(EEPROM.read(5));
  } else {
    m_mode = MODE_FIRST;
  }
}


void LedLamp::setBrightness(uint8_t brightness)
{
  if (brightness < 10)
  {
    brightness = 10;
  }
  if (brightness > 220) {
    brightness = 255;
  }

  m_brightness = brightness;
  FastLED.setBrightness(m_brightness);
};

void LedLamp::setParam1(uint8_t param1)
{
  m_param1 = param1;
};

void LedLamp::setParam2(uint8_t param2) {
  m_param2 = param2;
  m_currentPaletteValue = param2;

  m_currentPaletteValue = param2;
  int palette1 = param2 >> 5;
  int palette2 = palette1 + 1;
  if (palette2 > 7) {
    palette2 = 7;
  }  
  int ratio1 = 32 - (param2 % 32);
  int ratio2 = param2 % 32;
  for (int k = 0; k < 16; k++) {
    m_currentPalette.entries[k].r = (palettes[palette1].entries[k].r * ratio1 +  palettes[palette2].entries[k].r * ratio2) >> 5;
    m_currentPalette.entries[k].g = (palettes[palette1].entries[k].g * ratio1 +  palettes[palette2].entries[k].g * ratio2) >> 5;
    m_currentPalette.entries[k].b = (palettes[palette1].entries[k].b * ratio1 +  palettes[palette2].entries[k].b * ratio2) >> 5;
  }
}

void LedLamp::setParam3(uint8_t param3)
{
  m_param3 = param3;
};

void LedLamp::setParam4(uint8_t param4)
{
  m_param4 = param4;
};




void LedLamp::setColor(uint32_t index, CRGB color)
{
  uint8_t x = index % LEDS_PER_LEVEL;
  uint8_t y = index / LEDS_PER_LEVEL;

  if ((x & 1) == 0)
  {
    y = NUMBER_OF_LEVELS - y - 1;
  }

  uint32_t ledIndex = x * NUMBER_OF_LEVELS + y;

  leds[ledIndex] = color;
}

void LedLamp::nextMode()
{
  m_mode++;
  if (m_mode == MODE_LAST)
  {
    m_mode = MODE_FIRST;
  }
}

uint8_t LedLamp::render()
{
  uint8_t waitFor;
  switch (m_mode)
  {
  case MODE_STATIC:
    waitFor = renderStatic();
    break;
  case MODE_COLOR_CHANGE:
    waitFor = renderColorChange();
    break;
  case MODE_COLOR_CHANGE2:
    waitFor = renderColorChange2();
    break;
  case MODE_FIRE:
    waitFor = renderFire();
    break;
  case MODE_NOISE:
    waitFor = renderNoise();
    break;
  case MODE_SPARKLING:
    waitFor = renderSparkling();
    break;
  }
  FastLED.show();

  return waitFor;
}

inline void LedLamp::reduce(byte &aByte, byte aAmount, byte aMin)
{
  int r = aByte - aAmount;
  if (r < aMin)
    aByte = aMin;
  else
    aByte = (byte)r;
}

inline void LedLamp::increase(byte &aByte, byte aAmount, byte aMax)
{
  int r = aByte + aAmount;
  if (r > aMax)
    aByte = aMax;
  else
    aByte = (byte)r;
}

uint16_t LedLamp::random(uint16_t aMinOrMax, uint16_t aMax)
{
  if (aMax == 0)
  {
    aMax = aMinOrMax;
    aMinOrMax = 0;
  }
  uint32_t r = aMinOrMax;
  aMax = aMax - aMinOrMax + 1;
  r += rand() % aMax;
  return r;
}

uint8_t LedLamp::renderStatic()
{
  int green, blue;
  if (m_param1 <= 127) {
    green = 255;
    blue = 255 - m_param1 * 2;
  } else {
    blue = 0;
    green = 255 * 2 - m_param1 * 2;
  }
  CRGB color = CRGB(255, green, blue);
  for (int p = 0; p < NUMBER_OF_LEDS; p++)
  {
    leds[p] = color;
    //    leds[p] = CRGB(255, 0, 0);
    //leds[p] = CRGB(255, 255, 255);
  }

  return 20;
}


uint8_t LedLamp::renderSparkling()
{
  CRGB black = CRGB(0,0,0);
  for (int p = 0; p < NUMBER_OF_LEDS; p++)
  {
    CRGB color;
    if ((rand() % (32 - (m_param3 >> 3))) == 0) {
      color = m_currentPalette[rand() & 0xf];
    } else {
      color = black;
    }

    leds[p] = color;
    //    leds[p] = CRGB(255, 0, 0);
    //leds[p] = CRGB(255, 255, 255);
  }

  return 255 - m_param1;
}



uint8_t LedLamp::renderColorChange()
{
  m_hue += (m_param1 >> 6) + 1;
  float rainbowSpeed = m_param2 / 10.0f;
  for(int j = 0; j < NUMBER_OF_LEVELS; j++) {
    uint8_t hue = ((float)j * rainbowSpeed);
    hue += m_hue;
    for(int i = 0; i < LEDS_PER_LEVEL; i++) {
      if ((i & 1) == 0) {
        leds[i*NUMBER_OF_LEVELS + j].setHue(hue);
      } else {
        leds[i*NUMBER_OF_LEVELS + 36 - j].setHue(hue);
      }
    }
  }

  return 64 - (m_param1 >> 2);
}


uint8_t LedLamp::renderColorChange2()
{
  m_cpt += (m_param1 - 127) * 2;
  for(int j = 0; j < NUMBER_OF_LEVELS; j++) {
    uint16_t cpt = m_cpt + (m_param3 * 2) * j;

    for(int i = 0; i < LEDS_PER_LEVEL; i++) {
      uint8_t index1 = (cpt >>  8) & 0xf;
      uint8_t index2 = (index1 + 1) & 0xf;
      uint8_t ratio2 = cpt & 0xff;
      uint8_t ratio1 = 0xff - ratio2;
      CRGB color;
      color.r = (m_currentPalette.entries[index1].r * ratio1 +  m_currentPalette.entries[index2].r * ratio2) >> 8;
      color.g = (m_currentPalette.entries[index1].g * ratio1 +  m_currentPalette.entries[index2].g * ratio2) >> 8;
      color.b = (m_currentPalette.entries[index1].b * ratio1 +  m_currentPalette.entries[index2].b * ratio2) >> 8;
      if ((i & 1) == 0) {
        leds[i*NUMBER_OF_LEVELS + j] = color;
      } else {
        leds[i*NUMBER_OF_LEVELS + 36 - j] = color;
      }
      cpt += m_param4 * 3;
    }
  }

  return 20;
}




uint8_t LedLamp::renderNoise()
{
  uint16_t noisespeedx = m_param4 >> 4;
  uint16_t noisespeedy = 0;
  uint16_t noisespeedz = 1 + (m_param1 >> 4);
  uint16_t colorLoop = 0;

  // fillnoise8();


  // If we're runing at a low "speed", some 8-bit artifacts become visible
  // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
  // The amount of data smoothing we're doing depends on "speed".
  uint8_t dataSmoothing = 0;
  uint16_t lowestNoise = noisespeedx < noisespeedy ? noisespeedx : noisespeedy;
  lowestNoise = lowestNoise < noisespeedz ? lowestNoise : noisespeedz;
  if( lowestNoise < 8) {
    dataSmoothing = 200 - (lowestNoise * 4);
  }

  int k = 0;
  uint8_t noiseScale = m_param3 >> 1;
  if (noiseScale < 10) {
    noiseScale = 10;
  }

  for(int i = 0; i < LEDS_PER_LEVEL; i++) {
    int ioffset = noiseScale * i;
    for(int j = 0; j < NUMBER_OF_LEVELS; j++) {
      int joffset = noiseScale * j;

      uint8_t data = inoise8(noisex + ioffset, noisey + joffset, noisez);

      // The range of the inoise8 function is roughly 16-238.
      // These two operations expand those values out to roughly 0..255
      // You can comment them out if you want the raw noise data.
      data = qsub8(data,16);
      data = qadd8(data, scale8(data,39));

      if( dataSmoothing ) {
        uint8_t olddata = m_param[k].value;
        uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
        data = newdata;
      }

      m_param[k++].value = data;
    }
  }

  noisex += noisespeedx;
  noisey += noisespeedy;
  noisez += noisespeedz;


  // mapNoiseToLEDsUsingPalette(palette, hueReduce);
  static uint8_t ihue=0;
  k = 0;
  for(int i = 0; i < LEDS_PER_LEVEL; i++) {
    for(int j = 0; j < NUMBER_OF_LEVELS; j++) {
      // We use the value at the (i,j) coordinate in the noise
      // array for our brightness, and the flipped value from (j,i)
      // for our pixel's index into the color palette.

      uint8_t index = m_param[k].value;
      uint8_t bri =   m_param[NUMBER_OF_LEDS - k - 1].value;

      // if this palette is a 'loop', add a slowly-changing base value
      if( colorLoop) {
        index += ihue;
      }

      // brighten up, as the color palette itself often contains the
      // light/dark dynamic range desired
      if( bri > 127 ) {
        bri = 255;
      } else {
        bri = dim8_raw( bri * 2);
      }

      // if(hueReduce > 0) {
      //   if(index < hueReduce) index = 0;
      //   else index -= hueReduce;
      // }

      CRGB color = ColorFromPalette(m_currentPalette, index, bri);

      setColor(k++, color);
    }
  }

  ihue+=1;

  return 20;
}

uint8_t LedLamp::renderFire()
{


  int fireHeight = 2 + (m_param1 >> 3);
  // injectRandom();
  // random flame energy at bottom row
  for (int i = 0; i < LEDS_PER_LEVEL * fireHeight; i++)
  {
    m_param[i].currentEnergy = 6 + (rand() % 2);
    m_param[i].energyMode = torch_nop;
  }

  // random sparks at second row
  int sparkChance = 40 - (m_param1 >> 3); 
  for (int i = LEDS_PER_LEVEL * fireHeight; i < (fireHeight + 1) * LEDS_PER_LEVEL; i++)
  {
    if (m_param[i].energyMode != torch_spark && ((rand() % sparkChance) == 0))
    {
      m_param[i].currentEnergy = (rand() % 3) + 5;
      m_param[i].energyMode = torch_spark;
    }
  }

  // calcNextEnergy();
  int i = 0;
  for (int y = 0; y < NUMBER_OF_LEVELS; y++)
  {
    for (int x = 0; x < LEDS_PER_LEVEL; x++)
    {
      byte e = m_param[i].currentEnergy;
      byte m = m_param[i].energyMode;
      switch (m)
      {
      case torch_spark:
      {
        // loose transfer up energy as long as the is any
        reduce(e, 2);
        // cell above is temp spark, sucking up energy from this cell until empty
        if (y < NUMBER_OF_LEVELS - 1)
        {
          m_param[i + LEDS_PER_LEVEL].energyMode = torch_spark_temp;
        }
        break;
      }
      case torch_spark_temp:
      {
        // just getting some energy from below
        byte e2 = m_param[i - LEDS_PER_LEVEL].currentEnergy;
        if (e2 < 3)
        {
          // cell below is exhausted, becomes passive
          m_param[i - LEDS_PER_LEVEL].energyMode = torch_passive;
          // gobble up rest of energy
          increase(e, e2, 7);
          // loose some overall energy
          if ((rand() & 0x7) == 0) {
            reduce(e, 1);
          }
          // this cell becomes active spark
          m_param[i].energyMode = torch_spark;
        }
        else
        {
          increase(e, 1, 7);
        }
        break;
      }
      case torch_passive:
      {
        if (m_param[i - LEDS_PER_LEVEL].currentEnergy > 3) {
          e =  m_param[i - LEDS_PER_LEVEL].currentEnergy - 3;
        } else {
          e = 0;
        }
        break;
      }
      default:
        break;
      }
      m_param[i].nextEnergy = e ;
      i++;
    }
  }

  // calcNextColors();

  for (int i = 0; i < NUMBER_OF_LEDS; i++)
  {
    int ei = i;

    uint16_t e = m_param[ei].nextEnergy;
    m_param[ei].currentEnergy = e;
    
    if (e > 0)
    {
      // energy to brightness is non-linear
      uint16_t eb = energymap[e];

      uint16_t green = eb * 50;
      green >>= 8; 

      uint16_t red = eb * 180;
      red >>= 8;
      red += 20;
      
      setColor(i, CRGB(red, green, 0));
//      setColor(i, CRGB((int)40 + ((eb * 215) >> 8), (int)10 + ((eb * 180) >> 8), 0));
    } 
    // else if (e > 0)
    // {
    //   // energy to brightness is non-linear
    //   int eb = energymap[e];
    //   int red = 40 + ((eb * 140) >> 8);
    //   int green = ((eb * 30) >> 8);
    //   setColor(i, CRGB(red, green, 0));
    // } 
    else
    {
      // background, no energy
      setColor(i, CRGB(0, 0, 0));
    }
  }


  return 10;
}
