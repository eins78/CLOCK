
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

void wipeChar(int pos, uint8_t clr[], int wait) {
  for(int i=0; i<7; i++) {
    int pixel_pos = (display_map[pos - 1][i]) + display_start;
    strip.setPixelColor(pixel_pos, clr[0], clr[1], clr[2]);
    strip.show();
    delay(wait);
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

// -----------------------------------------------------------------------------
// analog controls functions
void colorMenu() {
  sensorVal = getAnalogInput(inputPin);

  // set HSB values
  switch (control_mode) {
    case 1:
      hue = map(sensorVal,4, 1019,0, 359); // hue is a number between 0 and 360
      getRGB(hue,saturation,brightness,rgb_colors);   // converts HSB to RGB
      break;
    case 2:
      saturation = map(sensorVal,4, 1019,0, 255); // saturation is a number between 0 - 255
      break;
    case 3:
      brightness = map(sensorVal,4, 1019,0, 255); // value is a number between 0 - 255
      break;
  }
  getRGB(hue,saturation,brightness,rgb_colors);   // converts HSB to RGB
}

void setControlMode(int button) {
  if (buttonPressed(button)) {
    control_mode++;
    if (control_mode > 3) control_mode = 0;
    if (control_mode == 1) {
      saturation = 255;
      brightness = 128;
    };
  }
}

int getAnalogInput(int inputPin) {
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

// debounced button:
bool buttonPressed(int buttonPin) {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);
  // Serial.println(reading);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
    lastButtonState = reading;
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
      if (reading == HIGH) return true;
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
// HSL to RGB stuff

/*
  Control a RGB led with Hue, Saturation and Brightness (HSB / HSV )

  Hue is change by an analog input.
  Brightness is changed by a fading function.
  Saturation stays constant at 255

  getRGB() function based on <http://www.codeproject.com/miscctrl/CPicker.asp>
  dim_curve idea by Jims <http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1174674545>

  created 05-01-2010 by kasperkamperman.com
*/


/*
  dim_curve 'lookup table' to compensate for the nonlinearity of human vision.
  Used in the getRGB function on saturation and brightness to make 'dimming' look more natural.
  Exponential function used to create values below :
  x from 0 - 255 : y = round(pow( 2.0, x+64/40.0) - 1)
*/

const byte dim_curve[] = {
    0,   1,   1,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,
    3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   4,   4,   4,   4,
    4,   4,   4,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,
    6,   6,   6,   6,   6,   7,   7,   7,   7,   7,   7,   7,   8,   8,   8,   8,
    8,   8,   9,   9,   9,   9,   9,   9,   10,  10,  10,  10,  10,  11,  11,  11,
    11,  11,  12,  12,  12,  12,  12,  13,  13,  13,  13,  14,  14,  14,  14,  15,
    15,  15,  16,  16,  16,  16,  17,  17,  17,  18,  18,  18,  19,  19,  19,  20,
    20,  20,  21,  21,  22,  22,  22,  23,  23,  24,  24,  25,  25,  25,  26,  26,
    27,  27,  28,  28,  29,  29,  30,  30,  31,  32,  32,  33,  33,  34,  35,  35,
    36,  36,  37,  38,  38,  39,  40,  40,  41,  42,  43,  43,  44,  45,  46,  47,
    48,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,
    63,  64,  65,  66,  68,  69,  70,  71,  73,  74,  75,  76,  78,  79,  81,  82,
    83,  85,  86,  88,  90,  91,  93,  94,  96,  98,  99,  101, 103, 105, 107, 109,
    110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
    146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
    193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255,
};

void getRGB(int hue, int sat, int val, uint8_t colors[3]) {
  /* convert hue, saturation and brightness ( HSB/HSV ) to RGB
     The dim_curve is used only on brightness/value and on saturation (inverted).
     This looks the most natural.
  */

  val = dim_curve[val];
  sat = 255-dim_curve[255-sat];

  int r;
  int g;
  int b;
  int base;

  if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
    colors[0]=val;
    colors[1]=val;
    colors[2]=val;
  } else  {

    base = ((255 - sat) * val)>>8;

    switch(hue/60) {
	case 0:
		r = val;
		g = (((val-base)*hue)/60)+base;
		b = base;
	break;

	case 1:
		r = (((val-base)*(60-(hue%60)))/60)+base;
		g = val;
		b = base;
	break;

	case 2:
		r = base;
		g = val;
		b = (((val-base)*(hue%60))/60)+base;
	break;

	case 3:
		r = base;
		g = (((val-base)*(60-(hue%60)))/60)+base;
		b = val;
	break;

	case 4:
		r = (((val-base)*(hue%60))/60)+base;
		g = base;
		b = val;
	break;

	case 5:
		r = val;
		g = base;
		b = (((val-base)*(60-(hue%60)))/60)+base;
	break;
    }

    colors[0]=r;
    colors[1]=g;
    colors[2]=b;
  }

}
