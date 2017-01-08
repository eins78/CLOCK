// arduino digital clock with 4 7-segment characters and 2 dots using neopixels
//
// based on/inspired by:
// - <http://playground.arduino.cc/Code/Time>
// - <https://github.com/vemeT5ak/7-segment-neopixel-clock/blob/4b1a8b56e2d49cf40c456f9cd0535f94f130c49a/_7SEG_COLOUR_CLOCK/supportFunctions.ino#L273-L296>

#include <Time.h>
#include <TimeLib.h>
#include <Adafruit_NeoPixel.h>

// display
#define DISPLAY_PIN 2
Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, DISPLAY_PIN, NEO_GRB + NEO_KHZ800);

// time serial sync
#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message

// analog controls
#define BUTTON_PIN 6
int control_mode = 0;
// button
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

// potentiometer
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
uint8_t rgb_colors[3] = {4, 0, 0}; // muted red

uint8_t main_color[3] = {4, 0, 0}; // muted red
static const uint8_t color_black[3] = {0, 0, 0}; // black/off

void setup()  {
  Serial.begin(9600);

  // controls
  pinMode(BUTTON_PIN, INPUT);
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  // display
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  startupShow(rgb_colors);
}

void loop(){

  if(Serial.available() ) {
    processSyncMessage();
  }

  setControlMode(BUTTON_PIN);

  if (control_mode > 0) {
    colorMenu();
    displaySetupMode(rgb_colors);
  } else {
    if(timeStatus() == timeNotSet) {
      // Serial.println("waiting for sync message");
      breathe(rgb_colors);
    }
    else {
      displayClock(now(), rgb_colors);
    }
  }
  delay(1);
}

void displaySetupMode(uint32_t clr) {
  setChar(1, 11, clr);
  setChar(2, 11, clr);
  setChar(3, 11, clr);
  setChar(4, 11, clr);
  setDots(clr);
  strip.show();
}

void displayClock(time_t ts, uint32_t clr) {
  int num1 = (int) (hour(ts) + 24) % 24;
  int num2 = (int) minute(ts);

  setChar(1, (num1 / 10), clr);
  setChar(2, (num1 % 10), clr);
  setChar(3, (num2 / 10), clr);
  setChar(4, (num2 % 10), clr);
  setDots(clr);
  strip.show();
}

void setDots(uint8_t clr[]) {
  strip.setPixelColor(28, clr[0], clr[1], clr[2]);
  strip.setPixelColor(29, clr[0], clr[1], clr[2]);
}

// display effects

void blink(uint8_t color[]) {
  int ms = millis();
  int blinking = abs((ms/1000) % 2);
  uint32_t clr = color_black;
  if (blinking) {
    clr = color;
  }

  setChar(1, 0, clr);
  setChar(2, 0, clr);
  setChar(3, 0, clr);
  setChar(4, 0, clr);
  setDots(clr);
  strip.show();
}

void breathe(uint8_t color[]) {
  // formula from http://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/
  // adapted to output 0-1 values for muting a color
  float val = (exp(sin(millis()/2000.0*PI)) - 0.36787944)*108.0/255;

  // mute color
  uint8_t clr[] = {(int) (color[0]*val), (int) (color[1]*val), (int) (color[2]*val)};

  // set digits and dots to 00:00 in muted color
  setChar(1, 0, clr);
  setChar(2, 0, clr);
  setChar(3, 0, clr);
  setChar(4, 0, clr);
  setDots(clr);
  strip.show();
}

void startupShow(uint8_t clr[]) {
  uint8_t c[] = {255,0,0};
  displayColorWipe(c);
  c[0] =   0; c[1] = 255, c[2] =   0; displayColorWipe(c);
  c[0] =   0; c[1] =   0, c[2] = 255; displayColorWipe(c);
  c[0] = 255; c[1] = 255, c[2] = 255; displayColorWipe(c);
  c[0] =   0; c[1] =   0, c[2] =   0; displayColorWipe(c);
  displayColorWipe(clr);
  delay(1000);
}

void displayColorWipe(uint8_t clr[]) {
  int wait = 50;
  wipeChar(1, clr, wait);
  wipeChar(2, clr, wait);
  strip.setPixelColor(28, clr[0], clr[1], clr[2]);
  strip.setPixelColor(29, clr[0], clr[1], clr[2]);
  wipeChar(3, clr, wait);
  delay(wait);
  wipeChar(4, clr, wait);
  delay(wait);
}
