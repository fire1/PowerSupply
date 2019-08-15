//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWER_SUPPLY_H
#define POWER_SUPPLY_H

#include <Arduino.h>
#include <ResponsiveAnalogRead.h>

#ifndef RESPONSIVE_ANALOG_READ_H

#include "../libraries/ResponsiveAnalogRead/src/ResponsiveAnalogRead.h"

#endif

#include <LiquidCrystal.h>




//Inputs/outputs
const uint8_t pinVolt = A1;
const uint8_t pinAmps = A0;
const uint8_t pinPWM = 5;
const uint8_t pinEncoderB = 2;
const uint8_t pinEncoderA = 3;
const uint8_t pinAnalogBt = A3;
const uint16_t screenRefresh = 310;
const uint8_t pinLed = 13;
const uint8_t pinThermistor = A6;
const uint8_t pinFanA = 10;//back fan
const uint8_t pinFanB = 11;// ic fan TODO change to tone
const uint16_t editTimeout = 10000;
const uint16_t holdTimeout = 400;
unsigned long previousMillis = 0;
volatile unsigned long currentLoops = 0;
uint8_t heat;

char printValues[6];

byte charLinear[] = {
        B00000,
        B11111,
        B00000,
        B10101,
        B00000,
        B00000,
        B00000,
        B00000
};
byte charLimits[] = {
        B00000,
        B11111,
        B00000,
        B01110,
        B01010,
        B01010,
        B11011,
        B00000
};
byte charSwitch[] = {
        B00000,
        B01110,
        B01010,
        B01010,
        B01010,
        B01010,
        B11011,
        B00000
};


unsigned long amplitude10, amplitude100, amplitude350;

boolean is10() {
    if (amplitude10 < currentLoops) {
        amplitude10 = currentLoops + 10;
        return true;
    }
}

boolean is80() {
    if (amplitude100 < currentLoops) {
        amplitude100 = currentLoops + 80;
        return true;
    }
}

boolean is250() {
    if (amplitude100 < currentLoops) {
        amplitude350 = currentLoops + 250;
        return true;
    }
}


void fansControl() {
    int rawTemp = analogRead(pinThermistor);
    heat = (uint8_t) map(rawTemp, 345, 460, 120, 70);

    if (heat > 25) {
        analogWrite(pinFanB, (int) map(heat, 80, 120, 0, 255));
        if (heat > 45) {
            analogWrite(pinFanA, (int) map(heat, 0, 120, 0, 255));
        }
        if (heat < 30) {
            analogWrite(pinFanA, 0);
        }
    } else if (heat < 22) {
        analogWrite(pinFanA, 0);
        analogWrite(pinFanB, 0);
    }
}

//Other Variables










#endif //POWERSUPPLY_POWERSUPPLY_H
