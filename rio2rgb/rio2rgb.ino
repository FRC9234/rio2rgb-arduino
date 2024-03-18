/*
Copyright 2024 FRC Team 9234 - Les aigles d'or - École Honoré-Mercier

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or other materials
provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be
used to endorse or promote products derived from this software without specific prior
written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <FastLED.h>

FASTLED_USING_NAMESPACE

#define DATA_PIN1 26
#define DATA_PIN2 25
#define DATA_PIN3 33
#define DATA_PIN4 32
//#define PWM1_PIN 1 //PCB V1
//#define PWM2_PIN 3 //PCB V1
#define PWM1_PIN 19  //PCB V2
#define PWM2_PIN 18  //PCB V2
#define WIFI_ENABLE_PIN 13

#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 100
#define FASTLED_ESP32_I2S true
#define DEFAULT_BRIGHTNESS 64
#define FRAMES_PER_SECOND 120

CRGB leds[NUM_LEDS];
uint8_t stripLength = NUM_LEDS;
long pwm1Value = 0; //pattern
long pwm2Value = 0; //brightness. but could be used to control patternAdj or patternSpeed
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
bool gReverseDirection = false;

uint8_t patternAdj = 3; //typical range = [0,30]
uint8_t patternSpeed = 5; //typical range = [0,30]
uint8_t pos = 0;
uint8_t prevpos = 0;
uint8_t startIndex = 0;

CRGB color1 = CRGB::Gold; //team palette color 1
CRGB color2 = CRGB::DarkBlue; //team palette color 2
TBlendType currentBlending = LINEARBLEND;  //NOBLEND;
CRGBPalette16 currentPalette = RainbowColors_p;
CRGBPalette16 teamPalette;

void setup() {
  delay(1000);
  Serial.begin(115200);
  pinMode(PWM1_PIN, INPUT);
  pinMode(PWM2_PIN, INPUT);
  FastLED.addLeds<LED_TYPE, DATA_PIN1, COLOR_ORDER>(leds, stripLength).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN2, COLOR_ORDER>(leds, stripLength).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN3, COLOR_ORDER>(leds, stripLength).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN4, COLOR_ORDER>(leds, stripLength).setCorrection(TypicalLEDStrip);

  teamPalette = SetupCustomPalette(color1, color2);
  currentPalette = RainbowColors_p;

  // set master brightness control
  FastLED.setBrightness(DEFAULT_BRIGHTNESS);
}


void loop() {
  EVERY_N_MILLISECONDS(20) {
    startIndex = startIndex + patternSpeed;
    gHue++;
  }

  EVERY_N_MILLISECONDS(250) {
    pwm1Value = normalizePulse(getPulse(PWM1_PIN));
    pwm2Value = getPulse(PWM2_PIN);
  } 

  if(pwm2Value != 0){
    FastLED.setBrightness(DEFAULT_BRIGHTNESS);
  } else {
    FastLED.setBrightness(constrain(map(pwm2Value, 1000, 2000, 0, 255),0,255));
  }

  switch(pwm1Value){
    case 1005: rainbow(RainbowColors_p); break;
    case 1015: rainbow(PartyColors_p); break;
    case 1025: rainbow(OceanColors_p); break;
    case 1035: rainbow(LavaColors_p); break;
    case 1045: rainbow(ForestColors_p); break;
    case 1055: rainbowWithGlitter(); break;
    case 1065: confetti(); break;
    case 1075: shot(CRGB::Red); break;
    case 1085: shot(CRGB::Blue); break;
    case 1095: shot(CRGB::White); break;
    case 1105: sinelon(RainbowColors_p); break;
    case 1115: sinelon(PartyColors_p); break;
    case 1125: sinelon(OceanColors_p); break;
    case 1135: sinelon(LavaColors_p); break;
    case 1145: sinelon(ForestColors_p); break;
    case 1155: bpm(RainbowColors_p); break;
    case 1165: bpm(PartyColors_p); break;
    case 1175: bpm(OceanColors_p); break;
    case 1185: bpm(LavaColors_p); break;
    case 1195: bpm(ForestColors_p); break;
    case 1205: Fire2012(90, 60); break;
    case 1215: Fire2012(120, 80); break;
    case 1225: drawTwinkles(RainbowColors_p); break;
    case 1235: drawTwinkles(PartyColors_p); break;
    case 1245: drawTwinkles(OceanColors_p); break;
    case 1255: drawTwinkles(LavaColors_p); break;
    case 1265: drawTwinkles(ForestColors_p); break;
    case 1275: colorwaves(RainbowColors_p); break;
    case 1285: colorwaves(PartyColors_p); break;
    case 1295: colorwaves(OceanColors_p); break;
    case 1305: colorwaves(LavaColors_p); break;
    case 1315: colorwaves(ForestColors_p); break;
    case 1325: larsonScanner(CRGB::DarkRed); break;
    case 1335: larsonScanner(CRGB::Gray); break;
    case 1345: lightChase(CRGB::DarkRed); break;
    case 1355: lightChase(CRGB::DarkBlue); break;
    case 1365: lightChase(CRGB::Gray); break;
    case 1375: heartBeat(CRGB::DarkRed, 1000); break;
    case 1385: heartBeat(CRGB::DarkBlue, 1000); break;
    case 1395: heartBeat(CRGB::White, 1000); break;
    case 1405: heartBeat(CRGB::Gray, 1000); break;
    case 1415: breath(CRGB::DarkRed, 1); break;
    case 1425: breath(CRGB::DarkBlue, 1); break;
    case 1435: breath(CRGB::Gray, 1); break;
    case 1445: strobe(CRGB::Red); break;
    case 1455: strobe(CRGB::Blue); break;
    case 1465: strobe(CRGB::Gold); break;
    case 1475: strobe(CRGB::White); break;
    case 1485: EndtoEndBlend(color1, CRGB::Black); break;
    case 1495: larsonScanner(color1); break;
    case 1505: lightChase(color1); break;
    case 1515: heartBeat(color1, 1500); break;
    case 1525: heartBeat(color1, 1000); break; 
    case 1535: heartBeat(color1, 800); break; 
    case 1545: breath(color1, 1); break;
    case 1555: breath(color1, 3); break;
    case 1565: shot(color1); break;
    case 1575: strobe(color1); break;  
    case 1585: EndtoEndBlend(color2, CRGB::Black); break;
    case 1595: larsonScanner(color2); break;
    case 1605: lightChase(color2); break;
    case 1615: heartBeat(color2, 1500); break;
    case 1625: heartBeat(color2, 1000); break; 
    case 1635: heartBeat(color2, 800); break; 
    case 1645: breath(color2, 1); break;
    case 1655: breath(color2, 3); break;
    case 1665: shot(color2); break;
    case 1675: strobe(color2); break;  
    case 1685: teamSparkle(color1, color2); break;
    case 1695: teamSparkle(color2, color1); break;
    case 1705: rainbow(teamPalette); break;
    case 1715: bpm(teamPalette); break;
    case 1725: EndtoEndBlend(color1, color2); break;
    case 1735: EndtoEndBlend(color2, color1); break;
    case 1745: testPattern(teamPalette); break;
    case 1755: drawTwinkles(teamPalette); break;
    case 1765: colorwaves(teamPalette); break;
    case 1775: sinelon(teamPalette); break;
    case 1785: fill_solid(leds, stripLength, CRGB::HotPink); break;
    case 1795: fill_solid(leds, stripLength, CRGB::DarkRed); break;
    case 1805: fill_solid(leds, stripLength, CRGB::Red); break;
    case 1815: fill_solid(leds, stripLength, CRGB::OrangeRed); break;
    case 1825: fill_solid(leds, stripLength, CRGB::Orange); break;
    case 1835: fill_solid(leds, stripLength, CRGB::Gold); break;
    case 1845: fill_solid(leds, stripLength, CRGB::Yellow); break;
    case 1855: fill_solid(leds, stripLength, CRGB::LawnGreen); break;
    case 1865: fill_solid(leds, stripLength, CRGB::Lime); break;
    case 1875: fill_solid(leds, stripLength, CRGB::DarkGreen); break;
    case 1885: fill_solid(leds, stripLength, CRGB::Green); break;
    case 1895: fill_solid(leds, stripLength, CRGB::SeaGreen); break;
    case 1905: fill_solid(leds, stripLength, CRGB::Aqua); break;
    case 1915: fill_solid(leds, stripLength, CRGB::SkyBlue); break;
    case 1925: fill_solid(leds, stripLength, CRGB::DarkBlue); break;
    case 1935: fill_solid(leds, stripLength, CRGB::Blue); break;
    case 1945: fill_solid(leds, stripLength, CRGB::BlueViolet); break;
    case 1955: fill_solid(leds, stripLength, CRGB::Violet); break;
    case 1965: fill_solid(leds, stripLength, CRGB::White); break;
    case 1975: fill_solid(leds, stripLength, CRGB::Gray); break;
    case 1985: fill_solid(leds, stripLength, CRGB::DarkGray); break;
    case 1995: fill_solid(leds, stripLength, CRGB::Black); break;
    default: rainbow(RainbowColors_p);

  }

  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);

}

long getPulse(byte pin) {
  return pulseIn(pin, HIGH, 25000UL);  // 25 millisecond timeout
}

//makes sure the received pulse is rounded to the nearest 5uS measurement
long normalizePulse(long pulse) {
  return pulse - (pulse % 10) + 5;
}

void displaySolid(CRGB color){
    fill_solid(leds, stripLength, color);
}

CRGBPalette16 SetupCustomPalette(CRGB color1, CRGB color2) {
  return CRGBPalette16(
    color1, color2, color1, color2,
    color1, color2, color1, color2,
    color1, color2, color1, color2,
    color1, color2, color1, color2);
}