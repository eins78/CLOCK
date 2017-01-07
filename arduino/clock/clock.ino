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

uint8_t main_color[3] = {22, 0, 0}; // muted red
static const uint8_t color_black[3] = {0, 0, 0}; // black/off

Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, DISPLAY_PIN, NEO_GRB + NEO_KHZ800);

void setup()  {
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop(){

  if(Serial.available() ) {
    processSyncMessage();
  }

  if(timeStatus() == timeNotSet) {
    breathe(main_color);
    // Serial.println("waiting for sync message");
    delay(1);
  }
  else {
    displayClock(now(), main_color);
    delay(10);
  }
}

// display helpers

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
  int ms = millis();

  // formula from http://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/
  // adapted to output 0-1 values for muting a color
  float val = (exp(sin(ms/2000.0*PI)) - 0.36787944)*108.0/255;

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


// display functions

/*
 __3__
|     |
2     6
|__4__|
|     |
1     7
|__5__|
0 not used as no decimal point
*/

static const unsigned char font[] = {
  0b01110111, // 0
  0b01100000, // 1
  0b01011101, // 2
  0b01111100, // 3
  0b01101010, // 4
  0b00111110, // 5
  0b00111111, // 6
  0b01100110, // 7
  0b01111111, // 8
  0b01111110, // 9
  0b00000000, // nothing
  0b11111111 // everything (debug/error)
};

// mapping from font to led positions, per display
static const int display_start = -1;
static const int display_map[4][7] = {
  { 12, 24, 28, 16,  4, 11, 23 }, // 1.
  { 10, 22, 27, 15,  3,  9, 21 }, // 2.
  {  8, 20, 26, 14,  2,  7, 19 }, // 3.
  {  6, 18, 25, 13,  1,  5, 17 }, // 4.
};

void setChar(int8_t pos, int8_t charcode, uint8_t clr[])
{
  //  get line from font
  uint8_t line = font[charcode];

  for (int i = 0; i<7; i++) {

    // get display pixel mapping for character at `pos`ition
    int pixel_pos = (display_map[pos - 1][i]) + display_start;

    if (line & 0b1) {
      strip.setPixelColor(pixel_pos, clr[0], clr[1], clr[2]);
    } else {
     strip.setPixelColor(pixel_pos, color_black[0], color_black[1], color_black[2]);
    }
    line >>= 1;
  }
}

void setDots(uint8_t clr[]) {
  strip.setPixelColor(28, clr[0], clr[1], clr[2]);
  strip.setPixelColor(29, clr[0], clr[1], clr[2]);
}

// serial clock sync

void processSyncMessage() {
  // if time sync available from serial port, update time and return true
  while(Serial.available() >=  TIME_MSG_LEN ){  // time message consists of header & 10 ASCII digits
    char c = Serial.read() ;
    Serial.print(c);
    if( c == TIME_HEADER ) {
      time_t pctime = 0;
      for(int i=0; i < TIME_MSG_LEN -1; i++){
        c = Serial.read();
        if( c >= '0' && c <= '9'){
          pctime = (10 * pctime) + (c - '0') ; // convert digits to a number
        }
      }
      if (pctime) setTime(pctime);   // Sync Arduino clock to the time received on the serial port
    }
  }
}
