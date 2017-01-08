#include <Adafruit_NeoPixel.h>
#define PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

// controls: analog input reading
int sensorVal = 0;                // the average sensor value
const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int inputPin = A0;

// color values
int hue;
int saturation;
int brightness;
int rgb_colors[3];

void setup() {
  Serial.begin(9600);

  // controls: initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop()  {
  sensorVal = getAnalogInput();

  // set HSB values
  hue        = map(sensorVal,4, 1019,0, 359);     // hue is a number between 0 and 360
  saturation = 255;                               // saturation is a number between 0 - 255
  brightness = 128;                           // value is a number between 0 - 255

  getRGB(hue,saturation,brightness,rgb_colors);   // converts HSB to RGB

  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, rgb_colors[0], rgb_colors[1], rgb_colors[2]);
  }
  strip.show();

  delay(20); // delay to slow down fading
}

int getAnalogInput() {
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(inputPin);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;
  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }
  // calculate the average:
  return total / numReadings;
}
