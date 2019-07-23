//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWERSUPPLY_POWERSUPPLY_H
#define POWERSUPPLY_POWERSUPPLY_H

#include <Arduino.h>

#define MAX_PWM_ByTimer 254

//Inputs/outputs
const uint8_t pinVolt = A0;
const uint8_t pinAmps = A1;
//const uint8_t currentIn = A0;
const uint8_t pinPWM = 5;
const uint8_t pinEncoderC = 0;
const uint8_t pinEncoderB = 0;
const uint8_t pinEncoderA = 1;
const uint16_t screenRate = 220;
// Common counter
volatile uint8_t index;


//Editable variables
float realAmpOffset = -0.03;
const float maxVoltage = 24.65;//28.15;


//Other Variables
uint8_t pwmValue = 1;
float targetVolt = 3.3;
float readVolts = 0;
float realVolts = 0;
int readAmps = 0;
float realCurrent = 0;
float realCurrentValue = 0;
float targetAmps = 4;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
// float mapDividerVolt = 42.2;
 float mapDividerVolt = 32;
uint16_t avrReadAmps;

int dumpVolts, dumpAmps ;

//
int val;
uint8_t encoderPos = 0;
uint8_t encoderPinALast = LOW;


#endif //POWERSUPPLY_POWERSUPPLY_H
