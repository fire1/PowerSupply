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
            container = val;
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

void sensVolts(){
    dumpVolts = readVolts = analogRead(pinVolt);
    realVolts = fmap(readVolts, 11, 379, 0.86, 10.5);
}

void sensAmps(){
    for (index = 0; index < 4; ++index) {
        avrReadAmps += analogRead(pinAmps);
    }
    dumpAmps = readAmps = avrReadAmps = avrReadAmps / 4;
    if (readAmps > 232) {
        realCurrent = map(readAmps, 232, 550, 900, 5300);
    } else {
        realCurrent = map(readAmps, 30, 232, 55, 900);
    }
    realCurrent = realCurrent < 0 ? 0 : realCurrent / 1000;
    realCurrentValue = realCurrentValue + realCurrent / 2;
}

uint8_t lastPwm = 0;

void setPwm(uint8_t pwm) {
    if (pwm == lastPwm) return;
    lastPwm = pwm;
    if (val > maxPwmControl) {
        val = maxPwmControl;
    }

    uint8_t compensate = 0;
    if (targetVolt <= (realVolts - 1) && pwm == maxPwmControl && lastPwm == pwm) {
        analogWrite(pinPWM, 0);
        return;
    }

    if (targetVolt >= (realVolts + 1) && pwm == maxPwmControl && lastPwm == pwm) {
        analogWrite(pinPWM, 255);
        return;
    }
    
    lastPwm = pwm;
    analogWrite(pinPWM, (maxPwmControl - pwm) - compensate);
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

void parse(){
    //If the set current value is higher than the feedback current value, we make normal control of output voltage
    if (realCurrentValue < targetAmps) {
        //Now, if set voltage is higher than real value from feedback, we decrease PWM width till we get same value
        if (targetVolt > realVolts) {
            //When we decrease PWM width, the voltage gets higher at the output.
            pwmValue = pwmValue - 1;
            pwmValue = constrain(pwmValue, 0, maxPwmControl);
        }
        //If set voltage is lower than real value from feedback, we increase PWM width till we get same value
        if (targetVolt < realVolts) {
            //When we increase PWM width, the voltage gets lower at the output.
            pwmValue = pwmValue + 1;
            pwmValue = constrain(pwmValue, 0, maxPwmControl);
        }
    }//end of realCurrentValue < targetAmps

    /*if the set current value is lower than the feedback current value, that means we need to lower the voltage at the output
    in order to amintain the same current value*/
    if (realCurrentValue > targetAmps) {
        //When we increase PWM width, the voltage gets lower at the output.
        pwmValue = pwmValue + 1;
        pwmValue = constrain(pwmValue, 0, maxPwmControl);
    }
}

/**
 *
 * @param out
 * @param value
 */
void lcdAmps(char *out, double value) {
    sprintf(out, "%1.3f", value);
}

/**
 *
 * @param out
 * @param value
 */
void lcdVolt(char *out, double value) {
    sprintf(out, "%2.2f", value);
}

/**
 *
 * @param lcdVolts
 * @param lcdAmps
 */
void display(){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" VOLTAGE    CURRENT ");

    lcd.setCursor(0, 1);
    lcd.print(targetVolt, 1);
    lcd.print("V       ");

    lcd.print(targetAmps, 0);
    lcd.print("mA");

    lcdAmps(printValues,realVolts);
    lcd.setCursor(0, 3);
    lcd.print(printValues, 1);
    lcd.print("V       ");

    lcdAmps(printValues,realCurrentValue);
    lcd.print(printValues, 0);
    //lcd.setCursor(19,1);
    lcd.print("А");
}

#endif //POWERSUPPLY_FUNCTIONS_H






