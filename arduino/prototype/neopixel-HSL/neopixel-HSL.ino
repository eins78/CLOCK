#include <Adafruit_NeoPixel.h>

// display
#define DISPLAY_PIN 2
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, DISPLAY_PIN, NEO_GRB + NEO_KHZ800);

// sensor values
int dials[3] = {0, 0, 0};

// color values
int hue;
int saturation;
int brightness;
int rgb_colors[3] = {0, 0, 0};

void setup() {
  Serial.begin(9600);

  // display
  strip.begin();
  for (int p = 0; p < 60; p++) {
    strip.setPixelColor(p, 0, 0, 0);
  }
  strip.show(); // Initialize all pixels to 'off'
}

// the loop routine runs over and over again forever:
void loop() {
  dials[0] = analogRead(A0);
  dials[1] = analogRead(A1);
  dials[2] = analogRead(A2);

  hue = 255 * (1-((float) dials[2]/1023.0));
  saturation = 100 * (1-((float) dials[0]/1023.0));
  brightness = 100 * (1-((float) dials[1]/1023.0));

  // hue = map(sensorVal,4, 1019,0, 359); // hue is a number between 0 and 360
  // saturation = map(sensorVal,4, 1019,0, 255); // saturation is a number between 0 - 255
  // brightness = map(sensorVal,4, 1019,0, 255); // value is a number between 0 - 255
  getRGB(hue,saturation,brightness,rgb_colors);   // converts HSB to RGB

  for (int i = 0; i < 10; i++) {
    strip.setPixelColor(i+20, rgb_colors[0], rgb_colors[1], rgb_colors[2]);
  }
  strip.show();

  // Serial.println(brightness);
  delay(10);
}
