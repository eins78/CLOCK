#define BUTTON_PIN 2
#define DISPLAY_PIN 6
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, DISPLAY_PIN, NEO_GRB + NEO_KHZ800);

// analog controls
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
int rgb_colors[3];

void setup() {
  Serial.begin(9600);

  // controls
  pinMode(BUTTON_PIN, INPUT);
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop()  {
  setControlMode(BUTTON_PIN);

  if (control_mode > 0) {
    colorMenu();
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, rgb_colors[0], rgb_colors[1], rgb_colors[2]);
    }
    strip.show();

  }
  delay(1);
}

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
