/// @file    arduinoLEDTest.ino
/// @brief   Uses @ref ColorPalettes to control LED strips and plays an animation on the built-in lED matrix. Also connects to wifi to change LED modes.
/// @example ColorPalette.ino

#include <FastLED.h>
#include "Waves.h"

#define LED_PIN     5
#define NUM_LEDS    200
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

// This example shows several ways to set up and use 'palettes' of colors
// with FastLED.
//
// These compact palettes provide an easy way to re-colorize your
// animation on the fly, quickly, easily, and with low overhead.
//
// USING palettes is MUCH simpler in practice than in theory, so first just
// run this sketch, and watch the pretty lights as you then read through
// the code.  Although this sketch has eight (or more) different color schemes,
// the entire sketch compiles down to about 6.5K on AVR.
//
// FastLED provides a few pre-configured color palettes, and makes it
// extremely easy to make up your own color schemes with palettes.
//
// Some notes on the more abstract 'theory and practice' of
// FastLED compact palettes are at the bottom of this file.



void setup() {
    delay( 3000 ); // power-up safety delay
    Serial.begin(9600);
    randomSeed(analogRead(0));
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
}


void loop()
{
    FillOceanWaves();
    FastLED.show();
}




void FillOceanWaves()
{
  static Waves ocean = Waves();
  ocean.updateWaves();
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ocean.getTopLEDAtIndex(i);
  }
}

