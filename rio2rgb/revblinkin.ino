/*
Adapted from Rev Blinkin Firmware by
FRC Team 9234 - Les aigles d'or - École Honoré-Mercier

Orginal code
https://github.com/REVrobotics/Blinkin-Firmware
All non addressable code removed

REV Robotics Non-Commercial License

Copyright (c) 2017 - 2020 REV Robotics LLC 
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted, provided that the following conditions are met:

1. Redistribution of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of REV Robotics nor the names of its contributors may
be used to endorse or promote products derived from this software without
specific prior written permission.

4. This software, including modifications and/or derivative works of this
software, must execute solely and exclusively on devices manufactured by
or for REV Robotics, unless in non-commercial use.

5. Redistribution and use of this software other than as permitted under
this license is void and will automatically terminate your rights under
this license.

THIS SOFTWARE IS PROVIDED BY REV ROBOTICS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
SHALL REV ROBOTICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE,EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

void rainbow(CRGBPalette16 palette) {
  uint8_t colorIndex = startIndex;
  uint8_t patternLength = map(patternAdj, 0, 30, 1, 10);

  for (int i = 0; i < stripLength; i++) {
    leds[i] = ColorFromPalette(palette, colorIndex, 255, currentBlending);
    colorIndex += patternLength;
  }
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

void rainbowWithGlitter() {
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow(RainbowColors_p);
  //addGlitter(80, CRGB::White);
  addGlitter(255, CRGB::White);
}

void addGlitter(fract8 chanceOfGlitter, CRGB gColor) {
  if (random8() < chanceOfGlitter) {
    leds[random8(stripLength)] = gColor;
  }
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

void confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, stripLength, 10);
  pos = random8(stripLength);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

void sinelon(CRGBPalette16 palette) {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, stripLength, 40);
  pos = beatsin8(15, 0, stripLength - 1);

  if (pos < prevpos) {
    fill_solid(leds + pos, (prevpos - pos) + 1, ColorFromPalette(palette, gHue));  //moveing back towards 0
  } else {
    fill_solid(leds + prevpos, (pos - prevpos) + 1, ColorFromPalette(palette, gHue));  //moving towards leds[stripLength]
  }
  prevpos = pos;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

void shot(CRGB gColor) {
  // Adjust the contrast ratio between the moving dot to the relative background
  uint8_t dimming = map(patternAdj, 0, 31, 0, 220);
  // Adjust the fill density and therefore relative speed of the moving dot
  uint8_t Speed = map(patternSpeed, 0, 31, 1, 5);

  pos = prevpos + Speed;

  if (pos >= stripLength) { pos = 0; }

  fadeToBlackBy(leds, stripLength, 10);

  for (int i = 0; i < stripLength; i++) {
    if ((random8() > 180) && (leds[i].getAverageLight() > 20))
      leds[i].addToRGB(leds[i].getAverageLight() * 2);
    leds[i].fadeToBlackBy(random8(40, 200));
  }

  fill_solid(leds + prevpos, (pos - prevpos) + 1, gColor);
  prevpos = pos;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

void bpm(CRGBPalette16 palette) {

  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = map(patternSpeed, 0, 30, 30, 140);  //62;
  uint8_t patternLength = map(patternAdj, 0, 30, 5, 30);

  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  //uint8_t beat = beatsin8( map(patternSpeed, 0, 30, 10, 200), 0, 255);

  for (int i = 0; i < stripLength; i++) {
    //leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i *30));
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * patternLength));
  }
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

// Adapted from Fire2012 by Mark Kriegsman
// FastLED Demo Code: https://github.com/FastLED/FastLED/tree/master/examples/Fire2012
void Fire2012(uint8_t SPARKING, uint8_t COOLING) {
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // SPARKING: What chance (out of 255) is there that a new spark will be lit?
  // Higher chance = more roaring fire.  Lower chance = more flickery fire.
  // Default 120, suggested range 50-200.
  //uint8_t SPARKING  = map(patternAdj, 0, 30, 50, 200); //120

  // COOLING: How much does the air cool as it rises?
  // Less cooling = taller flames.  More cooling = shorter flames.
  // Default 50, suggested range 20-100
  //uint8_t COOLING = map(patternSpeed, 0, 30, 20,100); //80

  bool gReverseDirection = false;
  currentPalette = LavaColors_p;

  // Step 1.  Cool down every cell a little
  for (int i = 0; i < stripLength; i++) {
    heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / stripLength) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (int k = stripLength - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if (random8() < SPARKING) {
    int y = random8(7);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }

  // Step 4.  Map from heat cells to LED colors
  for (int j = 0; j < stripLength; j++) {
    CRGB color = HeatColor(heat[j]);
    int pixelnumber;
    if (gReverseDirection) {
      pixelnumber = (stripLength - 1) - j;
    } else {
      pixelnumber = j;
    }
    leds[pixelnumber] = color;
  }
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

//Adapted from Mark Kriegsman's code "TwinkleFOX"
//https://gist.github.com/kriegsman/756ea6dcae8e30845b5a

// Background color for 'unlit' pixels
// Can be set to CRGB::Black if desired.
// CRGB gBackgroundColor = CRGB::Black;
// Example of dim incandescent fairy light background color
// CRGB gBackgroundColor = CRGB(CRGB::FairyLight).nscale8_video(16);

// If COOL_LIKE_INCANDESCENT is set to 1, colors will
// fade out slighted 'reddened', similar to how
// incandescent bulbs change color as they get dim down.
#define COOL_LIKE_INCANDESCENT 1

//  This function loops over each pixel, calculates the
//  adjusted 'clock' that this pixel should use, and calls
//  "CalculateOneTwinkle" on each pixel.  It then displays
//  either the twinkle color of the background color,
//  whichever is brighter.

void drawTwinkles(CRGBPalette16 palette) {
  currentPalette = palette;

  // "PRNG16" is the pseudorandom number generator
  // It MUST be reset to the same starting value each time
  // this function is called, so that the sequence of 'random'
  // numbers that it generates is (paradoxically) stable.
  uint16_t PRNG16 = 11337;

  uint32_t clock32 = millis();

  // Set up the background color, "bg".
  CRGB bg = CRGB::Black;  // just use the explicitly defined background color

  uint8_t backgroundBrightness = bg.getAverageLight();

  for (int i = 0; i < stripLength; i++) {
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384;  // next 'random' number
    uint16_t myclockoffset16 = PRNG16;          // use that number as clock offset
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384;  // next 'random' number
    // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
    uint8_t myspeedmultiplierQ5_3 = ((((PRNG16 & 0xFF) >> 4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
    uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
    uint8_t myunique8 = PRNG16 >> 8;  // get 'salt' value for this pixel

    // We now have the adjusted 'clock' for this pixel, now we call
    // the function that computes what color the pixel should be based
    // on the "255 = f( time )" idea.
    CRGB c = computeOneTwinkle(myclock30, myunique8);

    uint8_t cbright = c.getAverageLight();
    int16_t deltabright = cbright - backgroundBrightness;
    if (deltabright >= 32 || (!bg)) {
      // If the new pixel is significantly brighter than the background color,
      // use the new color.
      leds[i] = c;
    } else if (deltabright > 0) {
      // If the new pixel is just slightly brighter than the background color,
      // mix a blend of the new color and the background color
      leds[i] = blend(bg, c, deltabright * 8);
    } else {
      // if the new pixel is not at all brighter than the background color,
      // just use the background color.
      leds[i] = bg;
    }
  }
}

//  This function takes a time in pseudo-milliseconds,
//  figures out 255 = f( time ), and also hue = f( time )
//  The 'low digits' of the millisecond time are used as
//  input to the 255 wave function.
//  The 'high digits' are used to select a color, so that the color
//  does not change over the course of the fade-in, fade-out
//  of one cycle of the 255 wave function.
//  The 'high digits' are also used to determine whether this pixel
//  should light at all during this cycle, based on the TWINKLE_DENSITY.
CRGB computeOneTwinkle(uint32_t ms, uint8_t salt) {
  // Overall twinkle density.
  // 0 (NONE lit) to 8 (ALL lit at once).
  uint8_t TWINKLE_DENSITY = map(patternAdj, 0, 30, 1, 8);

  // Overall twinkle speed.
  // 0 (VERY slow) to 8 (VERY fast).
  // 4, 5, and 6 are recommended.
  uint8_t TWINKLE_SPEED = map(patternSpeed, 0, 30, 3, 7);

  uint16_t ticks = ms >> (8 - TWINKLE_SPEED);
  uint8_t fastcycle8 = ticks;
  uint16_t slowcycle16 = (ticks >> 8) + salt;
  slowcycle16 += sin8(slowcycle16);
  slowcycle16 = (slowcycle16 * 2053) + 1384;
  uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);

  uint8_t bright = 0;
  if (((slowcycle8 & 0x0E) / 2) < TWINKLE_DENSITY) {
    bright = attackDecayWave8(fastcycle8);
  }

  uint8_t hue = slowcycle8 - salt;
  CRGB c;
  if (bright > 0) {
    c = ColorFromPalette(currentPalette, hue, bright, NOBLEND);
    if (COOL_LIKE_INCANDESCENT == 1) {
      coolLikeIncandescent(c, fastcycle8);
    }
  } else {
    c = CRGB::Black;
  }
  return c;
}

// This function is like 'triwave8', which produces a
// symmetrical up-and-down triangle sawtooth waveform, except that this
// function produces a triangle wave with a faster attack and a slower decay:
//
//     / \ 
//    /     \ 
//   /         \ 
//  /             \ 
//

uint8_t attackDecayWave8(uint8_t i) {
  if (i < 86) {
    return i * 3;
  } else {
    i -= 86;
    return 255 - (i + (i / 2));
  }
}

// This function takes a pixel, and if its in the 'fading down'
// part of the cycle, it adjusts the color a little bit like the
// way that incandescent bulbs fade toward 'red' as they dim.
void coolLikeIncandescent(CRGB& c, uint8_t phase) {
  if (phase < 128) return;

  uint8_t cooling = (phase - 128) >> 4;
  c.g = qsub8(c.g, cooling);
  c.b = qsub8(c.b, cooling * 2);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

// Adapted from Mark Kriegsman's code "ColorWavesWithPalettes"
// https://gist.github.com/kriegsman/8281905786e8b2632aeb
void colorwaves(CRGBPalette16 palette) {
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint8_t sat8 = beatsin88(87, 220, 250);
  uint8_t brightdepth = beatsin88(341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;  //gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 300, 1500);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis;
  sLastMillis = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88(400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for (uint16_t i = 0; i < stripLength; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    uint16_t h16_128 = hue16 >> 7;
    if (h16_128 & 0x100) {
      hue8 = 255 - (h16_128 >> 1);
    } else {
      hue8 = h16_128 >> 1;
    }

    brightnesstheta16 += brightnessthetainc16;
    uint16_t b16 = sin16(brightnesstheta16) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    uint8_t index = hue8;
    index = scale8(index, 240);

    CRGB newcolor = ColorFromPalette(palette, index, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (stripLength - 1) - pixelnumber;

    nblend(leds[pixelnumber], newcolor, 128);
  }
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

//Adapted from:
//https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#library
void larsonScanner(CRGB gColor) {

  uint8_t eyeSize = map(patternAdj, 0, 31, 8, 30);
  uint8_t Speed = map(patternSpeed, 0, 31, 10, 35);

  pos = beatsin8(Speed, 0 + (eyeSize / 2), stripLength - (eyeSize / 2) - 1);

  fill_solid(leds, stripLength, CRGB::Black);

  fill_gradient_RGB(leds, pos - (eyeSize / 2), CRGB::Black, pos, gColor);
  fill_gradient_RGB(leds, pos, gColor, pos + (eyeSize / 2), CRGB::Black);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

// Adapted from:
// https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#library
void lightChase(CRGB gColor) {

  // Adjust the contrast ratio between the moving dot to the relative background
  uint8_t dimming = map(patternAdj, 0, 31, 0, 220);
  // Adjust the fill density and therefore relative speed of the moving dot
  uint8_t Speed = map(patternSpeed, 0, 31, 1, 5);

  pos = pos + Speed;
  if (pos >= stripLength) { pos = 0; }

  fill_solid(leds, stripLength, CRGB::Black);

  for (int i = 0; i < stripLength; i++) {
    leds[i] = gColor;
    //leds[i].fadeLightBy( dimming );
    leds[i].fadeToBlackBy(255 - dimming);
  }

  leds[pos] = gColor;
  leds[pos].maximizeBrightness();

  //prevpos = pos;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

// Adapted from Marc Miller's "heart_pulse_blood_flowing"
// https://github.com/marmilicious/FastLED_examples/blob/master/heart_pulse_blood_flowing.ino#L26

#define PULSELENGTH 300
#define PULSEOFFSET 280

void heartBeat(CRGB gColor, uint16_t cycleLength) {

  uint8_t bloodVal = qadd8(pulseWave8(millis(), cycleLength, PULSELENGTH - 100), pulseWave8(millis() + PULSEOFFSET, cycleLength, PULSELENGTH));

  for (int i = 0; i < stripLength; i++) {
    leds[i] = gColor;
    leds[i].fadeLightBy(255 - bloodVal);
  }
}

uint8_t pulseWave8(uint32_t ms, uint16_t cycleLength, uint16_t pulseLength) {
  uint8_t baseBrightness = 20;  // 255 of LEDs when not pulsing. Set to 0 for off.

  uint16_t T = ms % cycleLength;
  if (T > pulseLength) return baseBrightness;

  uint16_t halfPulse = pulseLength / 2;
  if (T <= halfPulse) {
    return (T * 255) / halfPulse;  //first half = going up
  } else {
    return ((pulseLength - T) * 255) / halfPulse;  //second half = going down
  }
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

//Adapted from Stuart Taylor's "WS2811 - breathe demo"
//https://gist.github.com/hsiboy/4eae11073e9d5b21eae3
void breath(CRGB gColor, uint8_t Speed) {
  fill_solid(leds, stripLength, gColor.fadeLightBy(constrain(255 - ((exp(sin((millis() * Speed) / 2000.0 * PI)) - 0.36787944) * 108.0), 0, 235)));
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

void strobe(CRGB gColor) {
  //uint8_t Speed = map(patternSpeed, 0, 31, 80, 255);
  if (beat8(255) > 80) {
    //beatsin8( 220, 0, 255) > 150){
    displaySolid(gColor);
    //leds.maximize255();
  } else {
    displaySolid(CRGB::Black);
  }
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

void EndtoEndBlend(CRGB gColor1, CRGB gColor2) {
  uint8_t speed = beatsin8(6, 0, 255);
  CRGB endclr = blend(gColor2, gColor1, speed);
  CRGB midclr = blend(gColor1, gColor2, speed);

  fill_gradient_RGB(leds, 0, endclr, stripLength / 2, midclr);
  fill_gradient_RGB(leds, stripLength / 2 + 1, midclr, stripLength - 1, endclr);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

void teamSparkle(CRGB gColor1, CRGB gColor2) {
  fill_solid(leds, stripLength, gColor1);
  fadeToBlackBy(leds, stripLength, 100);
  addGlitter(255, gColor2);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

void testPattern(CRGBPalette16 palette) {
  //uint8_t colorIndex = 1;

  for (int i = 0; i < stripLength; i++) {
    leds[i] = ColorFromPalette(palette, i%16, 255, NOBLEND);
    //colorIndex += 3;
  }
}
