//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWER_SUPPLY_FUNCTIONS_H
#define POWER_SUPPLY_FUNCTIONS_H

#include <Arduino.h>
#include "../PowerSupply.h"


class Controller {
    volatile uint8_t index;
    volatile uint8_t offset;
    uint8_t pwmValue = 1;
    float targetVolt = 3.3;
    float liveVolts = 0;
    float readVolts = 0;
    uint16_t avrReadAmps;
    int dumpVolts, dumpAmps;
    int readAmps = 0;
    double liveAmps = 0;
    double targetAmps = 0.500;
    double ampSmooth = 0;

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
        // liveVolts = fmap(readVolts, 11, 379, 0.86, 10.5);
        liveVolts = map(readVolts, 140, 940, 411, 2700) * 0.01;
    }

// https://circuits4you.com/2016/05/13/dc-current-measurement-arduino/
// V=I x R
    void testAmps() {
        int refVolt = 940; // Max volt read as reference
        int refAmps = 550; // Max amps read as reference;
        double bla = ((readVolts / refVolt) * (avrReadAmps / refAmps)) * 10;
    }

    void sensAmps() {
        for (index = 0; index < 4; ++index) {
            avrReadAmps += analogRead(pinAmps);
        }
        dumpAmps = readAmps = avrReadAmps = avrReadAmps / 4;

        testAmps();

        if (readAmps > 232) {
            liveAmps = map(readAmps, 232, 550, 900, 5300);
        } else {
            liveAmps = map(readAmps, 30, 232, 55, 900);
        }
        liveAmps = liveAmps < 0 ? 0 : liveAmps * 0.001;
        ampSmooth = liveAmps / 2;
    }

    uint8_t lastPwm = 0;

    void setPwm(uint8_t pwm) {

        if (lastPwm == pwm && pwm == maxPwmControl) {
//        pwm -=1;
        }

        uint8_t compensate = 0;
        if (targetVolt > (liveVolts - 2) && pwm == maxPwmControl && lastPwm == pwm) {
            compensate = 0;
        }

        if (targetVolt <= (liveVolts + 1)) {
//        pwmValue--;
        }
        lastPwm = pwm;
        analogWrite(pinPWM, pwm);
    }


    void parsePwm() {
        //If the set current value is higher than the feedback current value, we make normal control of output voltage
        if (liveAmps < targetAmps) {
            //Now, if set voltage is higher than real value from feedback, we decrease PWM width till we get same value
            if (targetVolt > liveVolts) {
                //When we decrease PWM width, the voltage gets higher at the output.
                pwmValue = pwmValue + 1;
                pwmValue = constrain(pwmValue, minPwmControl, maxPwmControl);
            }
            //If set voltage is lower than real value from feedback, we increase PWM width till we get same value
            if (targetVolt < liveVolts) {
                //When we increase PWM width, the voltage gets lower at the output.
                pwmValue = pwmValue - 1;
                pwmValue = constrain(pwmValue, minPwmControl, maxPwmControl);
            }
        }

        /*if the set current value is lower than the feedback current value, that means we need to lower the voltage at the output
        in order to amintain the same current value*/
        if (liveAmps > targetAmps) {
            //When we increase PWM width, the voltage gets lower at the output.
            pwmValue = pwmValue - 1;
            pwmValue = constrain(pwmValue, minPwmControl, maxPwmControl);
        }
    }

public:

//    Controller(const uint8_t pwm, const uint8_t inVolt, const uint8_t inAmps,){
//
//    }

    void begin() {
        setupPwm();
    }

    void manage() {
        sensVolts();
        sensAmps();

        if ((liveVolts > targetVolt + 1 || liveVolts < targetVolt - 1)) {
            offset = 0;
            while (liveVolts > targetVolt + 1 && liveVolts < targetVolt - 1 && offset < 228) {
                sensVolts();
                parsePwm();
                setPwm(pwmValue);
                digitalWrite(pinLed, HIGH);
                delayMicroseconds(10);
                offset++;
            }
        }

        digitalWrite(pinLed, LOW);
        parsePwm();
        pinMode(pinVolt, INPUT);
        pinMode(pinAmps, INPUT);
        pinMode(pinLed, OUTPUT);
        analogWrite(pinPWM, 0);
        setPwm(pwmValue);
    }

    void setVoltage(float value) {
        targetVolt = value;
    }

    void setAmperage(float value) {
        targetAmps = value;
    }

    float getLiveVolt() {
        return liveVolts;
    }

    float getLiveAmps() {
        return liveAmps;
    }

    int getDumpVolt() {
        return dumpVolts;
    }

    int getDumpAmps() {
        return dumpAmps;
    }

    float getTargetVolt() {
        return targetVolt;
    }

    float getTargetAmps() {
        return targetAmps;
    }

    uint8_t getPwmValue() {
        return pwmValue;
    }


    double lcdVolt(){
        return liveVolts;
    }

    double lcdAmps(){
        return ampSmooth;
    }
};


#endif //POWERSUPPLY_FUNCTIONS_H






