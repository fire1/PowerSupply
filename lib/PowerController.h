//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWER_SUPPLY_FUNCTIONS_H
#define POWER_SUPPLY_FUNCTIONS_H

#include <Arduino.h>
#include "../PowerSupply.h"


class PowerController {
    volatile uint8_t index;
    volatile uint8_t offset;
    uint8_t pwmValue = 1;
    float targetVolt = 3.3;
    float liveVolts = 0;
    float readVolts = 0;
    uint16_t avrReadAmps;
    uint32_t ampSmoothIndex;
    int dumpVolts, dumpAmps;
    int readAmps = 0;
    double liveAmps = 0;
    double targetAmps = 0.500;
    double ampSmooth = 0;

    double testAmp;

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


        //---------------------------------------------- Set PWM frequency for D9 & D10 ------------------------------
//        TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
//        TCCR1B = TCCR1B & B11111000 | B00000101;    // set timer 1 divisor to  1024 for PWM frequency of    30.64 Hz
//        TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
    }

    void sensVolts() {
        dumpVolts = readVolts = analogRead(pinVolt);
        // liveVolts = fmap(readVolts, 11, 379, 0.86, 10.5);
        liveVolts = map(readVolts, 140, 940, 411, 2700) * 0.01;
    }

// 6.25
// https://circuits4you.com/2016/05/13/dc-current-measurement-arduino/
// V=I x R
    void testAmps() {
//        int refVolt = 940; // Max volt read as reference
//        int refAmps = 550; // Max amps read as reference;
//        testAmp = ((readVolts / refVolt) * (avrReadAmps / refAmps)) * 10;

        float deflectVolt =
                map((int) liveVolts, 0, 30, 935, 1196/*1036*/ /*11.96428*/) * 0.1; // deflection curve by voltage
        testAmp = readAmps * deflectVolt * 0.1;

    }

    void sensAmps() {
        for (index = 0; index < 4; ++index) {
            avrReadAmps += analogRead(pinAmps);
            delayMicroseconds(1);
        }
        dumpAmps = readAmps = avrReadAmps = avrReadAmps / 4;
        testAmps();


        if (readAmps < 30) {
            liveAmps = map(readAmps, 19, 40, 120, 250);
        } else if (readAmps < 61) {
            liveAmps = map(readAmps, 20, 45, 205, 630);
        } else if (readAmps > 60) {
            liveAmps = map(readAmps, 56, 550, 670, 5700);
        }
//        liveAmps = liveAmps < 0 ? 0 : liveAmps * 0.001;
        liveAmps = testAmp * 0.001;
        ampSmooth += liveAmps;
        ampSmoothIndex++;
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
/**
 *
 * @param pin
 * @param divisor
 */
    void setPwmFrequency(int pin, int divisor) {
        byte mode;
        if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
            switch(divisor) {
                case 1: mode = 0x01; break;
                case 8: mode = 0x02; break;
                case 64: mode = 0x03; break;
                case 256: mode = 0x04; break;
                case 1024: mode = 0x05; break;
                default: return;
            }
            if(pin == 5 || pin == 6) {
                TCCR0B = TCCR0B & 0b11111000 | mode;
            } else {
                TCCR1B = TCCR1B & 0b11111000 | mode;
            }
        } else if(pin == 3 || pin == 11) {
            switch(divisor) {
                case 1: mode = 0x01; break;
                case 8: mode = 0x02; break;
                case 32: mode = 0x03; break;
                case 64: mode = 0x04; break;
                case 128: mode = 0x05; break;
                case 256: mode = 0x06; break;
                case 1024: mode = 0x7; break;
                default: return;
            }
            TCCR2B = TCCR2B & 0b11111000 | mode;
        }
    }

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
        if (value >= 0)
            targetVolt = value;
    }

    void setAmperage(float value) {
        if (value >= 0)
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


    double lcdVolt() {
        return liveVolts;
    }

    double lcdAmps() {
        double result = ampSmooth / ampSmoothIndex;
        ampSmooth = result;
        ampSmoothIndex = 1;
        return result;
    }


    double testAmperage() {
        return testAmp;
    }
};


#endif //POWERSUPPLY_FUNCTIONS_H






