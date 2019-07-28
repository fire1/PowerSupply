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


void sensVolts() {
    dumpVolts = readVolts = analogRead(pinVolt);
    // realVolts = fmap(readVolts, 11, 379, 0.86, 10.5);
    realVolts = map(readVolts, 140, 940, 411, 2700) * 0.01;
}

void sensAmps() {
    for (index = 0; index < 4; ++index) {
        avrReadAmps += analogRead(pinAmps);
    }
    dumpAmps = readAmps = avrReadAmps = avrReadAmps / 4;
    if (readAmps > 232) {
        realCurrent = map(readAmps, 232, 550, 900, 5300);
    } else {
        realCurrent = map(readAmps, 30, 232, 55, 900);
    }
    realCurrent = realCurrent < 0 ? 0 : realCurrent * 0.001;
    realCurrentValue = realCurrent / 2;
}

uint8_t lastPwm = 0;

void setPwm(uint8_t pwm) {

    if (lastPwm == pwm && pwm == maxPwmControl) {
//        pwm -=1;
    }

    uint8_t compensate = 0;
    if (targetVolt > (realVolts - 2) && pwm == maxPwmControl && lastPwm == pwm) {
        compensate = 0;
    }

    if (targetVolt <= (realVolts + 1)) {
//        pwmValue--;
    }
    lastPwm = pwm;
    analogWrite(pinPWM, pwm);
}


void parse() {
    //If the set current value is higher than the feedback current value, we make normal control of output voltage
    if (realCurrentValue < targetAmps) {
        //Now, if set voltage is higher than real value from feedback, we decrease PWM width till we get same value
        if (targetVolt > realVolts) {
            //When we decrease PWM width, the voltage gets higher at the output.
            pwmValue = pwmValue + 1;
            pwmValue = constrain(pwmValue, minPwmControl, maxPwmControl);
        }
        //If set voltage is lower than real value from feedback, we increase PWM width till we get same value
        if (targetVolt < realVolts) {
            //When we increase PWM width, the voltage gets lower at the output.
            pwmValue = pwmValue - 1;
            pwmValue = constrain(pwmValue, minPwmControl, maxPwmControl);
        }
    }//end of realCurrentValue < targetAmps

    /*if the set current value is lower than the feedback current value, that means we need to lower the voltage at the output
    in order to amintain the same current value*/
    if (realCurrentValue > targetAmps) {
        //When we increase PWM width, the voltage gets lower at the output.
        pwmValue = pwmValue - 1;
        pwmValue = constrain(pwmValue, minPwmControl, maxPwmControl);
    }
}


char displayVolt(float value, char *output) {

    if (value < -99) {
        value = -99;
    }

    int dig1 = int(value) * 10; // 210
    int dig2 = int((value * 10) - dig1);

    dig1 = dig1 / 10;
    if (dig2 < 0) {
        dig2 = dig2 * -1;
    }

    sprintf(output, "%02d.%02d", dig1, dig2);
}

char displayAmps(double value, char *output) {

    if (value < -99) {
        value = -99;
    }

    int dig1 = int(value) * 100; // 210
    int dig2 = int((value * 100) - dig1);

    dig1 = dig1 / 100;
    if (dig2 < 0) {
        dig2 = dig2 * -1;
    }

    sprintf(output, "%01d.%03d", dig1, dig2);
}


/**
 *
 * @param out
 * @param value
 */
void lcdAmps(char *out, double value) {
    dtostrf(value, 1, 3, out);
}

/**
 *
 * @param out
 * @param value
 */
void lcdVolt(char *out, double value) {
    /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
    dtostrf(value, 2, 2, out);
}


void display() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" VOLTAGE    CURRENT ");

    lcd.setCursor(1, 1);
    lcd.print(targetVolt, 1);
    lcd.print("V       ");

    lcd.print(targetAmps, 0);
    lcd.print("A");

//    lcdVolt(printValues, realVolts);
    lcd.setCursor(0, 3);
    lcd.print(printValues);
    lcd.print("V       ");

//    lcdAmps(printValues, realCurrentValue);
    lcd.print(printValues);
    //lcd.setCursor(19,1);
    lcd.print("А");
}


void debug() {

    Serial.println();
    Serial.print(F("Amp In: "));
    displayAmps(realCurrent, printValues);
    Serial.write(printValues);
    Serial.print(F(" Raw "));
    Serial.print(dumpAmps);
    Serial.print(F(" T: "));
    Serial.print(targetAmps);

    Serial.print(F(" //  Volt In: "));
    displayVolt(realVolts, printValues);
    Serial.print(printValues);
    Serial.print(F(" Raw "));
    Serial.print(dumpVolts);
    Serial.print(F(" T: "));
    Serial.print(targetVolt);

    Serial.print(F(" PWM: "));
    Serial.print(pwmValue);

}

#endif //POWERSUPPLY_FUNCTIONS_H






