#define PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

const int sensorPin = 0;  // pin the potmeter is attached too
int sensorVal = 0; // store the value coming from the sensor

const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

int inputPin = A0;


int hue;
int saturation;
int brightness;
int rgb_colors[3];

void setup() {
  Serial.begin(57600);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop()  {
  sensorVal = analogRead(sensorPin);

  // set HSB values
  hue        = map(sensorVal,0, 1023,0, 359);     // hue is a number between 0 and 360
  saturation = 255;                               // saturation is a number between 0 - 255
  brightness = 128;                           // value is a number between 0 - 255

  getRGB(hue,saturation,brightness,rgb_colors);   // converts HSB to RGB

  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, rgb_colors[0], rgb_colors[1], rgb_colors[2]);
  }
  strip.show();

  delay(20); // delay to slow down fading
}

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

void getRGB(int hue, int sat, int val, int colors[3]) {
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