#include <math.h>
#include <Adafruit_NeoPixel.h>

#define PIN 2
Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  int ms = millis();
  
  // formula from http://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/
  float val = (exp(sin(ms/2000.0*PI)) - 0.36787944)*108.0;  
  
  // breathe digits in red
  uint32_t color = strip.Color((int) val, 0, 0);
  for(uint16_t i=0; i<28; i++) {
    strip.setPixelColor(i, color);
  }

  // blink separator dots
  int blinking = abs((ms/1000) % 2) * 255;
  strip.setPixelColor(28, strip.Color(blinking, 0, 0));
  strip.setPixelColor(29, strip.Color(blinking, 0, 0));

  
  strip.show();
}

