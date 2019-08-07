//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWER_SUPPLY_H
#define POWER_SUPPLY_H

#include <Arduino.h>
#include <LiquidCrystal.h>

#ifndef LiquidCrystal_h

#include "../libraries/LiquidCrystal/src/LiquidCrystal.h"

#endif

#define maxPwmControl 254
#define minPwmControl 1

//Inputs/outputs
const uint8_t pinVolt = A1;
const uint8_t pinAmps = A0;
const uint8_t pinPWM = 5;
const uint8_t pinEncoderB = 2;
const uint8_t pinEncoderA = 3;
const uint8_t pinAnalogBt = A3;
const uint16_t screenRefresh = 350;
const uint8_t pinLed = 13;
const uint8_t pinThermistor = A6;
const uint8_t pinFanA = 10;//back fan
const uint8_t pinFanB = 11;// ic fan

unsigned long previousMillis = 0;
volatile unsigned long currentMillis = 0;
uint8_t heat;

char printValues[6];

byte charLinear[] = {
        B01000,
        B01000,
        B01000,
        B01100,
        B00000,
        B11111,
        B00000,
        B10101
};

byte charSwitch[] = {
        B00100,
        B01000,
        B00100,
        B01000,
        B00000,
        B10101,
        B00000,
        B10101
};


//Other Variables










#endif //POWERSUPPLY_POWERSUPPLY_H
