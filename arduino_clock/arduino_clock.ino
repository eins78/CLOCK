#include <Time.h>  
#include <Adafruit_NeoPixel.h>

#define PIN 2
Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, PIN, NEO_GRB + NEO_KHZ800);

// from <http://playground.arduino.cc/Code/Time>
#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

// T1262347200  //noon Jan 1 2010

void setup()  {
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop(){    
  // fake clock
//  setTime(0);
  
  if(Serial.available() ) {
//    processSyncMessage();
  }
  
//  if(timeStatus() == timeNotSet) 
//    Serial.println("waiting for sync message");
//  else     
//      digitalClockDisplay();  
  int ms = millis();
  
//  Serial.print(ms);
//  Serial.print(' . ');
//  Serial.prinln((int) abs((ms/1000) % 10));
//  Serial.print(' . ');
//  Serial.print((int) abs((ms/1000) % 100));
//  Serial.println();
  
//  Serial.println((int) abs((ms/1000) % 10));
//  setChar(0, (int) abs((ms/1000) % 10));
  delay(1000);

setChar(0, 0);
setChar(1, 1);
setChar(2, 7);
setChar(3, 8);
}

//void printDigits(int digits){
//  // utility function for digital clock display: prints preceding colon and leading 0
//  Serial.print(":");
//  if(digits < 10)
//    Serial.print('0');
//  Serial.print(digits);
//}

//void processSyncMessage() {
//  // if time sync available from serial port, update time and return true
//  while(Serial.available() >=  TIME_MSG_LEN ){  // time message consists of header & 10 ASCII digits
//    char c = Serial.read() ; 
//    Serial.print(c);  
//    if( c == TIME_HEADER ) {       
//      time_t pctime = 0;
//      for(int i=0; i < TIME_MSG_LEN -1; i++){   
//        c = Serial.read();          
//        if( c >= '0' && c <= '9'){   
//          pctime = (10 * pctime) + (c - '0') ; // convert digits to a number    
//        }
//      }   
//      setTime(pctime);   // Sync Arduino clock to the time received on the serial port
//    }  
//  }
//}



// from <https://github.com/vemeT5ak/7-segment-neopixel-clock/blob/4b1a8b56e2d49cf40c456f9cd0535f94f130c49a/_7SEG_COLOUR_CLOCK/supportFunctions.ino#L273-L296>

/*
 __5__
|     |
6     1
|__4__|
|     |
7     2
|__3__|
0 not used as no decimal point
*/

// font is an array, "charcode" is index in this array
//static const unsigned char font[] = {
//  0b01110111, //0
//  0b01100000, //1
//  0b01011101, //2
//  0b01111100, //3
//  0b01101010, //4
//  0b00111110, //5
//  0b00111111, //6
//  0b01100100, //7
//  0b01111111, //8
//  0b01111110, //9
//  0b00000000 //nothing
//};

static const unsigned char font[] = {
  0b01110111, //0
  0b01100000, //1
  0b01011101, //2
  0b01111100, //3
  0b01101010, //4
  0b00111110, //5
  0b00111111, //6
  0b01100110, //7
  0b01111111, //8
  0b01111110, //9
  0b00000000 //nothing
};

// mapping from font to led positions, per display
static const int display_start = -1;
static const int display_map[4][7] = {
//  { 23, 11,  4, 16, 28, 24, 12 }, // 1.
  { 12, 24,  28, 16, 4, 11, 23 }, // 1.
  {  9, 21, 26, 14,  2, 10, 22 }, // 2.
  {  7, 19, 27, 15,  3,  8, 20 }, // 3.
  {  5, 17, 28, 16,  4,  6, 18 }, // 4.
};

void setChar(int8_t pos, int8_t charcode)
{    
  //  get line from font
  uint8_t line = font[charcode];
  
  for (int i = 0; i<7; i++) {
    
    // get display pixel mapping for character at `pos`ition
    int pixel_pos = (display_map[pos][i]) + display_start;
    Serial.print(pixel_pos);
  
    if (line & 0b1) {
      Serial.print(" on. ");
      strip.setPixelColor(pixel_pos, strip.Color(128, 0, 0)); // red
    } else {
     Serial.print(" off. ");
     strip.setPixelColor(pixel_pos, strip.Color(0, 0, 0)); // black
    }
    line >>= 1;
  }
  
//  Serial.println();
  strip.show();
}
