//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWERSUPPLY_FUNCTIONS_H
#define POWERSUPPLY_FUNCTIONS_H

#include <Arduino.h>

#include "header.h"

void encoder() {
    uint8_t n = digitalRead(pinEncoderA);
    if ((encoderPinALast == LOW) && (n == HIGH)) {
        if (digitalRead(pinEncoderB) == LOW) {
            encoderPos--;
        } else {
            encoderPos++;
        }
        Serial.print(encoderPos);
        Serial.print("/");
    }
    encoderPinALast = n;

}

void terminal(float &container) {
    if (Serial.available()) {
        float val = Serial.parseFloat(); //read int or parseFloat for ..float...
        if (val > 0) {
//            if (val > 24) {
//                container = 24;
//                return;
//            }
            container = val;

//            OCR2B = (uint8_t)val;
//            Serial.println();
//            Serial.print(F("Set point is "));
//            Serial.println(val);
        }
    }
}

void setupPwm() {
    pinMode(pinPWM, OUTPUT);
    //---------------------------------------------- Set PWM frequency for D5 & D6 -------------------------------

    TCCR0B = TCCR0B & B11111000 | B00000001;    // set timer 0 divisor to     1 for PWM frequency of 62500.00 Hz
//    TCCR0B = TCCR0B & B11111000 | B00000010;    // set timer 0 divisor to     8 for PWM frequency of  7812.50 Hz
//    TCCR0B = TCCR0B & B11111000 | B00000011;    // set timer 0 divisor to    64 for PWM frequency of   976.56 Hz
//TCCR0B = TCCR0B & B11111000 | B00000100;    // set timer 0 divisor to   256 for PWM frequency of   244.14 Hz
//TCCR0B = TCCR0B & B11111000 | B00000101;    // set timer 0 divisor to  1024 for PWM frequency of    61.04 Hz

    //---------------------------------------------- Set PWM frequency for D3 & D11 ------------------------------

//    TCCR2B = TCCR2B & B11111000 | B00000001;    // set timer 2 divisor to     1 for PWM frequency of 31372.55 Hz
//TCCR2B = TCCR2B & B11111000 | B00000010;    // set timer 2 divisor to     8 for PWM frequency of  3921.16 Hz
//TCCR2B = TCCR2B & B11111000 | B00000011;    // set timer 2 divisor to    32 for PWM frequency of   980.39 Hz
//    TCCR2B = TCCR2B & B11111000 | B00000100;    // set timer 2 divisor to    64 for PWM frequency of   490.20 Hz (The DEFAULT)
//TCCR2B = TCCR2B & B11111000 | B00000101;    // set timer 2 divisor to   128 for PWM frequency of   245.10 Hz
//TCCR2B = TCCR2B & B11111000 | B00000110;    // set timer 2 divisor to   256 for PWM frequency of   122.55 Hz
//TCCR2B = TCCR2B & B11111000 | B00000111;    // set timer 2 divisor to  1024 for PWM frequency of    30.64 Hz

}


uint8_t lastPwm = 0;

void setPwm(uint8_t pwm) {
    if (pwm == lastPwm) return;
    lastPwm = pwm;
    if (val > MAX_PWM_ByTimer) {
        val = MAX_PWM_ByTimer;
    }

    uint8_t compensate = 0;
    if (targetVolt < (realVolts - 1) && pwm == MAX_PWM_ByTimer && lastPwm == pwm) {
        analogWrite(pinPWM, 0);
        return;
    }
    lastPwm = pwm;

    analogWrite(pinPWM, (MAX_PWM_ByTimer - pwm) - compensate);
//    OCR2B = (uint8_t)MAX_PWM_ByTimer - (pwm - 1);
//    Serial.println(pwm -1);
}

//THIS FUNCTION WILL MAP THE float VALUES IN THE GIVEN RANGE
float fmap(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void debug() {

    Serial.println();
    Serial.print(F("Amp In: "));
    Serial.print(realCurrentValue);
    Serial.print(F(" Raw "));
    Serial.print(dumpAmps);
    Serial.print(F(" T: "));
    Serial.print(targetAmps);

    Serial.print(F(" //  Volt In: "));
    Serial.print(realVolts);
    Serial.print(F(" Raw "));
    Serial.print(dumpVolts);
    Serial.print(F(" T: "));
    Serial.print(targetVolt);

    Serial.print(F(" PWM: "));
    Serial.print(pwmValue);

}

#endif //POWERSUPPLY_FUNCTIONS_H






