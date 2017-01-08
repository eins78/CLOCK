// arduino digital clock with 4 7-segment characters and 2 dots using neopixels
//
// based on/inspired by:
// - <http://playground.arduino.cc/Code/Time>
// - <https://github.com/vemeT5ak/7-segment-neopixel-clock/blob/4b1a8b56e2d49cf40c456f9cd0535f94f130c49a/_7SEG_COLOUR_CLOCK/supportFunctions.ino#L273-L296>

#include <Time.h>
#include <TimeLib.h>
#include <Adafruit_NeoPixel.h>

#define DISPLAY_PIN 2
#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message

uint8_t main_color[3] = {4, 0, 0}; // muted red
static const uint8_t color_black[3] = {0, 0, 0}; // black/off

Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, DISPLAY_PIN, NEO_GRB + NEO_KHZ800);

void setup()  {
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  startupShow(main_color);
}

void loop(){

  if(Serial.available() ) {
    processSyncMessage();
  }

  if(timeStatus() == timeNotSet) {
    breathe(main_color);
    // Serial.println("waiting for sync message");
  }
  else {
    displayClock(now(), main_color);
  }
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
