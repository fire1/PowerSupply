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
//const uint8_t currentIn = A0;
const uint8_t pinPWM = 5;
const uint8_t pinEncoderC = 0;
const uint8_t pinEncoderB = 2;
const uint8_t pinEncoderA = 3;
const uint16_t screenRate = 16000;
const uint8_t pinLed = 13;
// Common counter



//Editable variables
float realAmpOffset = -0.03;
const float maxVoltage = 24.65;//28.15;



char printValues[6];

//Other Variables










#endif //POWERSUPPLY_POWERSUPPLY_H
