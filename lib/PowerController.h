//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWER_SUPPLY_FUNCTIONS_H
#define POWER_SUPPLY_FUNCTIONS_H

#include <Arduino.h>
#include "../PowerSupply.h"


ResponsiveAnalogRead rawVolt(pinVolt, true);
ResponsiveAnalogRead rawAmps(pinAmps, true);

//  pwm     / volt
//
//  100         26
//  62          25
//  61          21
//  60          18
//  59          17.4
//  57          16.1
//  56          14.6
//  54          13.1
//  53          12.3
//  52          11
//  50          8.6
//  49          7
//  47          5.6
//  46          4.8
//  45          4.1
//  44          3.2

#ifndef maxPwmValue
#define maxPwmValue  255;
#endif

#ifndef minPwmValue
#define minPwmValue 1
#endif

class PowerController {

    const static uint8_t voltIndex = 26;
    boolean activeParse = true;
    boolean activeTable = true;
    volatile uint8_t index;
    volatile uint8_t offset;
    uint8_t powerMode = 0; // liner, power switching, limited switching
    uint8_t pwmValue = 1;
    uint8_t maxPwmControl = maxPwmValue;
    uint8_t minPwmControl = minPwmValue;
    uint8_t lastTrVoltage = 0;
    float targetVolt = 3;
    float liveVolts = 0;
    float readVolts = 0;
    uint32_t ampSmoothIndex;
    int dumpVolts, dumpAmps;
    int readAmps = 0;
    double liveAmps = 0;
    double targetAmps = 0.200;
    double ampSmooth = 0;

    double testAmp;
    // Pwm table for voltage
    uint8_t voltTable[voltIndex] = {
            100, 62, 62, 62, 62, 61, 61, 61, 60, 59, 57, 57, 56, 54, 53, 52, 51, 51, 50, 49, 48, 47, 45, 44, 40, 30
    };

    void setupPwm() {
        pinMode(pinPWM, OUTPUT);
        //---------------------------------------------- Set PWM frequency for D5 & D6 -------------------------------

        pinMode(3, OUTPUT); // Output pin for OCR2B
        pinMode(5, OUTPUT); // Output pin for OCR0B

//        TCCR0B = TCCR0B & 0b11111000 | 0x05;
        TCCR0B = TCCR0B & B11111000 | B00000001;    // set timer 0 divisor to     1 for PWM frequency of 62500.00 Hz
//    TCCR0B = TCCR0B & B11111000 | B00000010;    // set timer 0 divisor to     8 for PWM frequency of  7812.50 Hz
//    TCCR0B = TCCR0B & B11111000 | B00000011;    // set timer 0 divisor to    64 for PWM frequency of   976.56 Hz
//TCCR0B = TCCR0B & B11111000 | B00000100;    // set timer 0 divisor to   256 for PWM frequency of   244.14 Hz
//TCCR0B = TCCR0B & B11111000 | B00000101;    // set timer 0 divisor to  1024 for PWM frequency of    61.04 Hz

        //---------------------------------------------- Set PWM frequency for D3 & D11 ------------------------------

        TCCR2B = TCCR2B & B11111000 | B00000001;    // set timer 2 divisor to     1 for PWM frequency of 31372.55 Hz
//TCCR2B = TCCR2B & B11111000 | B00000010;    // set timer 2 divisor to     8 for PWM frequency of  3921.16 Hz
//TCCR2B = TCCR2B & B11111000 | B00000011;    // set timer 2 divisor to    32 for PWM frequency of   980.39 Hz
//    TCCR2B = TCCR2B & B11111000 | B00000100;    // set timer 2 divisor to    64 for PWM frequency of   490.20 Hz (The DEFAULT)
//TCCR2B = TCCR2B & B11111000 | B00000101;    // set timer 2 divisor to   128 for PWM frequency of   245.10 Hz
//TCCR2B = TCCR2B & B11111000 | B00000110;    // set timer 2 divisor to   256 for PWM frequency of   122.55 Hz
//TCCR2B = TCCR2B & B11111000 | B00000111;    // set timer 2 divisor to  1024 for PWM frequency of    30.64 Hz


        //---------------------------------------------- Set PWM frequency for D9 & D10 ------------------------------
        TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
//        TCCR1B = TCCR1B & B11111000 | B00000101;    // set timer 1 divisor to  1024 for PWM frequency of    30.64 Hz
//        TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
    }


    void readValues() {
        for (index = 0; index < 10; ++index) {
            rawVolt.update();
            rawAmps.update();
            delayMicroseconds(1);
        }
    }

    void sensVolts() {
        readValues();
        dumpVolts = readVolts = rawVolt.getValue();
        liveVolts = map(readVolts, 140, 940, 411, 2700) * 0.01;
    }


    void sensAmps() {
        dumpAmps = readAmps = rawAmps.getValue();
        float deflectVolt = map((int) liveVolts, 0, 30, 935, 1196) * 0.1; // deflection curve by voltage
        testAmp = readAmps * deflectVolt * 0.1;


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


    void resolveMaxPwmValue() {
        if (lastTrVoltage != targetVolt && activeTable) {
            maxPwmControl = voltTable[uint8_t(targetVolt)];
        } else if (!activeTable) {
            maxPwmControl = maxPwmValue;
        }
        lastTrVoltage = uint8_t(targetVolt);
    }

/**
 *
 * @param pwm
 */
    void setPwm(uint8_t pwm) {
        lastPwm = pwm;
        analogWrite(pinPWM, pwm);
    }


    void parsePwmSwitching() {
        if (!activeParse) return;
        resolveMaxPwmValue();
        //If the set current value is higher than the feedback current value, we make normal control of output voltage
        if (liveAmps < targetAmps) {
            //Now, if set voltage is higher than real value from feedback, we decrease PWM width till we get same value
            if (targetVolt > liveVolts) {
                //When we decrease PWM width, the voltage gets higher at the output.
                pwmValue = pwmValue + 1;
                pwmValue = constrain(pwmValue, minPwmControl, maxPwmControl);
            }

            //If set voltage is lower than real value from feedback, we increase PWM width till we get same value
            if (targetVolt < liveVolts - 0.2) {
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
        if (pin == 5 || pin == 6 || pin == 9 || pin == 10) {
            switch (divisor) {
                case 1:
                    mode = 0x01;
                    break;
                case 8:
                    mode = 0x02;
                    break;
                case 64:
                    mode = 0x03;
                    break;
                case 256:
                    mode = 0x04;
                    break;
                case 1024:
                    mode = 0x05;
                    break;
                default:
                    return;
            }
            if (pin == 5 || pin == 6) {
                TCCR0B = TCCR0B & 0b11111000 | mode;
            } else {
                TCCR1B = TCCR1B & 0b11111000 | mode;
            }
        } else if (pin == 3 || pin == 11) {
            switch (divisor) {
                case 1:
                    mode = 0x01;
                    break;
                case 8:
                    mode = 0x02;
                    break;
                case 32:
                    mode = 0x03;
                    break;
                case 64:
                    mode = 0x04;
                    break;
                case 128:
                    mode = 0x05;
                    break;
                case 256:
                    mode = 0x06;
                    break;
                case 1024:
                    mode = 0x7;
                    break;
                default:
                    return;
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

                parsePwmSwitching();
                sensVolts();
                setPwm(pwmValue);
                digitalWrite(pinLed, HIGH);
                delayMicroseconds(3);
                offset++;
            }
        }

        digitalWrite(pinLed, LOW);
        parsePwmSwitching();
        pinMode(pinVolt, INPUT);
        pinMode(pinAmps, INPUT);
        pinMode(pinLed, OUTPUT);
        analogWrite(pinPWM, 0);
        setPwm(pwmValue);
    }

    void setVoltage(float value) {
        if (value >= 0 && value <= 26)
            targetVolt = value;
    }

    void setAmperage(float value) {
        if (value >= 0 && value <= 10)
            targetAmps = value;
    }

/**
 *
 * @param value
 */
    void setPWM(uint8_t value) {
        pwmValue = value;
    }

    float getLiveVolt() {
        return liveVolts;
    }

    float getLiveAmps() {
        return liveAmps;
    }

    void useParse(boolean active) {
        activeParse = active;
    }


    void togglePwmMode() {
        this->activeTable = !this->activeTable;
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






