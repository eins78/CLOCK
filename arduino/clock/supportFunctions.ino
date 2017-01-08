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

void wipeChar(int pos, uint8_t clr[], int wait) {
  for(int i=0; i<7; i++) {
    int pixel_pos = (display_map[pos - 1][i]) + display_start;
    strip.setPixelColor(pixel_pos, clr[0], clr[1], clr[2]);
    strip.show();
    delay(wait);
  }
}

// -----------------------------------------------------------------------------
// display "driver" functions

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

// -----------------------------------------------------------------------------
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
