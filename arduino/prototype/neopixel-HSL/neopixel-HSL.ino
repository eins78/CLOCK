#include <Adafruit_NeoPixel.h>

// display
#define DISPLAY_PIN 2
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, DISPLAY_PIN, NEO_GRB + NEO_KHZ800);

// analog inputs
const int numDials = 5;
const int dialsNumReadings = 10;
const int dialsInputPinStart = A0;
size_t dials[numDials];         // the average sensor values
int dialsReading[numDials][dialsNumReadings];  // the readings from the analog input
int dialsReadIndex = 0;                 // the index of the current readings
float dialsTotal[numDials];    // the running totals

// color values
int hue;
int saturation;
int brightness;
int rgb_colors[3] = {0, 0, 0};

void setup() {
  Serial.begin(9600);

  setupAnalogInputs(numDials, dialsInputPinStart, dialsNumReadings);

  strip.begin();
  for (int p = 0; p < 60; p++) {
    strip.setPixelColor(p, 0, 0, 0);
  }
  strip.show();
}

void loop() {

  readAnalogInputs(dialsInputPinStart, numDials);

  hue = mapValue(dials[2], 990, 50, 0, 359);             // hue is a number between 0 and 360
  saturation = mapValue(dials[0], 990, 50, 0, 255);      // saturation is a number between 0 - 255
  brightness = mapValue(dials[1], 990, 50, 0, 255);      // value is a number between 0 - 255
  getRGB(hue, saturation, brightness, rgb_colors);       // converts HSB to RGB

  for (int i = 0; i < 10; i++) {
    strip.setPixelColor(i+20, rgb_colors[0], rgb_colors[1], rgb_colors[2]);
  }
  strip.show();

  // serial debug
  char colorDebugString[40];
  sprintf(
    colorDebugString,
    "H%03d, S%03d, B%03d \| R%03d, G%03d, B%03d",
    (int) hue, (int) saturation, (int) brightness,
    (int) rgb_colors[0], (int) rgb_colors[1], (int) rgb_colors[2]
  );
  Serial.println(colorDebugString);

  delay(10);
}

void setupAnalogInputs(int numInputs, int startPin, int numReadings) {
  // initialize all the readings to 0:
  for (int thisInput = 0; thisInput < numInputs; thisInput++) {
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
      dialsReading[thisInput][thisReading] = 0;
    }
  }
}

void readAnalogInputs(int inputPinStart, int numInputs) {
  for (int num = 0; num < numInputs; num++) {
    dials[num] = getAnalogInput(num, inputPinStart + num);
  }
  // advance to the next position in the array, wrap around to the beginning:
  dialsReadIndex = dialsReadIndex + 1;
  if (dialsReadIndex >= dialsNumReadings) { dialsReadIndex = 0; }
}

int getAnalogInput(int num, int inputPin) {
  // subtract the last reading:
  dialsTotal[num] = dialsTotal[num] - dialsReading[num][dialsReadIndex];
  // read from the sensor:
  dialsReading[num][dialsReadIndex] = analogRead(inputPin);
  // add the reading to the total:
  dialsTotal[num] = dialsTotal[num] + dialsReading[num][dialsReadIndex];
  // calculate the average:
  return (float) dialsTotal[num] / (float) dialsNumReadings;
}

long mapValue(int x, int in_min, int in_max, int out_min, int out_max)
{
  return min(max((((float) x - (float) in_min) * ((float) out_max - (float) out_min) / ((float) in_max - (float) in_min) + (float) out_min), out_min), out_max);
}
