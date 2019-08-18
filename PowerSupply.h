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
boolean fastScreen = false;
const uint8_t pinVolt = A1;
const uint8_t pinAmps = A0;
const uint8_t pinSwhPwm = 5;
const uint8_t pinLinPwm = 6;
const uint8_t pinEncoderB = 2;
const uint8_t pinEncoderA = 3;
const uint8_t pinAnalogBt = A3;
const uint16_t screenNormalRefresh = 310;
const uint16_t screenEditorRefresh = 120;
const uint8_t pinLed = 13;
const uint8_t pinThermistorSwt = A6;
const uint8_t pinThermistorLin = A2;
const uint8_t pinFans = 10;
const uint8_t pinTone = 11;
const uint16_t editTimeout = 10000;
const uint16_t holdTimeout = 400;
unsigned long previousMillis = 0;
volatile unsigned long currentLoops = 0;
uint8_t heatSwt, heatLin;


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
    int rawTempSwt = analogRead(pinThermistorSwt);
    int rawTempLin = analogRead(pinThermistorLin);
    heatSwt = (uint8_t) map(rawTempSwt, 345, 460, 120, 70);
    heatLin = (uint8_t) map(rawTempLin, 345, 460, 120, 70);

    uint8_t heat = (heatLin > heatSwt) ? heatLin : heatSwt;
    if (heat > 30) {
        analogWrite(pinFans, (int) map(heat, 30, 120, 0, 254));
    } else if (heatSwt < 25 && heatLin < 25) {
        analogWrite(pinFans, 0);
    }
}

//Other Variables










#endif //POWERSUPPLY_POWERSUPPLY_H
