// based on `dcf77Interrupt` example from ELV <>
#include <stdio.h>
#include "DateTime.h"
#include "RealTimeClock_DCF.h"

#include "modSoftwareSerial.h"
//#include <SPI.h>
//#include <Wire.h>

#include <EnableInterrupt.h>

// globals
DateTime readDateTime;
volatile uint8_t perInterruptFlag = 0;
volatile uint8_t dcf77InterruptFlag = 0;

void setup() {
  delay(1500); // wait for module startup

  // arduino: periodicInterrupt
  pinMode(RTC_DCF_PER_INT_PIN, INPUT);
  enableInterrupt(RTC_DCF_PER_INT_PIN, &periodicInterrupt, FALLING);
  // arduino: dcf77Interrupt
  pinMode(RTC_DCF_DCF77_INT_PIN, INPUT);
  enableInterrupt(RTC_DCF_DCF77_INT_PIN, &dcf77Interrupt, FALLING);

  // init module
  RTC_DCF.begin();

  RTC_DCF.enableDCF77Reception();
  RTC_DCF.enableDCF77Interrupt();

  // module: periodicInterrupt
  RTC_DCF.setPeriodicInterruptMode(RTC_PERIODIC_INT_PULSE_1_HZ);
  // module: dcf77Interrupt
  RTC_DCF.enablePeriodicInterrupt();

  Serial.begin(115200);

  interrupts();
}

void loop() {
  if(perInterruptFlag == 1) {
    RTC_DCF.getDateTime(&readDateTime);
    printClock();

    perInterruptFlag = 0;
  }

  if(dcf77InterruptFlag == 1) {
    Serial.println("DCF77 empfangen");

    RTC_DCF.getDateTime(&readDateTime);

    RTC_DCF.resetDCF77Interrupt();
    printClock();

    dcf77InterruptFlag = 0;
  }
}

void periodicInterrupt(void) {
  perInterruptFlag = 1;
}

void dcf77Interrupt(void) {
  dcf77InterruptFlag = 1;
}

void printClock(void) {
  char clockString[30];

  sprintf(
    clockString,
    "%02u-%02u-%02u %02u:%02u:%02u",
    readDateTime.getYear(),readDateTime.getMonth(), readDateTime.getDay(),
    readDateTime.getHour(), readDateTime.getMinute(), readDateTime.getSecond()
  );

  Serial.println(clockString);
}
