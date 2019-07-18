//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWERSUPPLY_POWERSUPPLY_H
#define POWERSUPPLY_POWERSUPPLY_H

#include <Arduino.h>

//Inputs/outputs
const uint8_t pinVolt = A2;
const uint8_t pinAmps = A3;
const uint8_t currentIn = A0;
const uint8_t pinPWM = 6;
const uint8_t pinEncoderB = 4;
const uint8_t pinEncoderA = 3;
const uint16_t screenRate = 320;
// Common counter
volatile uint8_t index;


//Editable variables
float realAmpOffset = -0.03;
const float maxVoltage = 15.0;


//Other Variables
int pwmValue = 1;
float targetVolt = 1.3;
float readVolts = 0;
float realVolts = 0;
float RawValue = 0;
float realCurrent = 0;
float realCurrentValue = 0;
float targetAmps = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
float mapDivider = 0;

//
int val;
uint8_t encoderPos = 0;
uint8_t encoderPinALast = LOW;


#endif //POWERSUPPLY_POWERSUPPLY_H
