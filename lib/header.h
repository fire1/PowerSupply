//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWERSUPPLY_POWERSUPPLY_H
#define POWERSUPPLY_POWERSUPPLY_H

#include <Arduino.h>
#include <LiquidCrystal.h>

#ifndef LiquidCrystal_h

#include "../../libraries/LiquidCrystal/src/LiquidCrystal.h"

#endif

#define maxPwmControl 254
#define minPwmControl 1

//Inputs/outputs
const uint8_t pinVolt = A0;
const uint8_t pinAmps = A1;
//const uint8_t currentIn = A0;
const uint8_t pinPWM = 5;
const uint8_t pinEncoderC = 0;
const uint8_t pinEncoderB = 2;
const uint8_t pinEncoderA = 3;
const uint16_t screenRate = 16000;
const uint8_t pinLed = 13;
// Common counter
volatile uint8_t index,offset;


//Editable variables
float realAmpOffset = -0.03;
const float maxVoltage = 24.65;//28.15;


//Other Variables
uint8_t pwmValue = 1;
float targetVolt = 3.3;
float readVolts = 0;
float realVolts = 0;
int readAmps = 0;
double realCurrent = 0;
double targetAmps = 0.500;
char printValues[6];
double realCurrentValue = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
uint16_t avrReadAmps;

int dumpVolts, dumpAmps;

//
int val;
uint8_t encoderPos = 0;
uint8_t encoderPinALast = LOW;

//   LiquidCrystal(uint8_t rs, uint8_t enable,

// 2, 3 INTERRUPTS for rotary
LiquidCrystal lcd(1, 0, 4, 7, 8, 9);

#endif //POWERSUPPLY_POWERSUPPLY_H
