//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWER_SUPPLY_FUNCTIONS_H
#define POWER_SUPPLY_FUNCTIONS_H

#include <Arduino.h>
#include "../PowerSupply.h"


ResponsiveAnalogRead rawVolt(pinVolInp, true);
ResponsiveAnalogRead rawAmps(pinAmpInp, true);

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


#define avrSimples 100
#define avrGap 50

#ifndef maxPwmValue
#define maxPwmValue  200 //200
#endif

#ifndef minPwmValue
#define minPwmValue 3
#endif

#ifndef pwmSwtCorrection
#define pwmSwtCorrection 0.30
#endif

// voltage table
// V    Value
// 1    15
// 2    23
// 3    31
// 4    39
// 5    43
// 6    55
// 7    62
// 8    70
// 9    78
// 10   86
// 11   94
// 12   101
// 13   109
// 14   117
// 15   125
// 16   132
// 17   140
// 18   148
// 19   156
// 20   164
// step 8

class PowerController {

    boolean isPowered = true;
    uint8_t setVolt, setAmps, lastVolt, lastAmps;
    float outVolt = 3.0;
    float outAmps = 0.100;

    void setupPwm() {

    }

public:

    static const uint8_t MODE_SWT_PW = 0;
    static const uint8_t MODE_SWT_LM = 1;
    static const uint8_t MODE_LIN_PW = 2;
    static const uint8_t MODE_LIN_LM = 3;


    void begin() {
        setupPwm();
        pinMode(pinVolInp, INPUT);
        pinMode(pinAmpInp, INPUT);
        pinMode(pinLed, OUTPUT);

        pinMode(pinVolPwm, OUTPUT);
        pinMode(pinAmpPwm, OUTPUT);
        TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
    }

    void manage() {
        if (!isPowered) {
            return;
        }
        if (lastAmps != setAmps) {
            analogWrite(pinAmpPwm, setAmps);
            lastAmps = setAmps;
        }
        if (lastVolt != setVolt) {
            analogWrite(pinVolPwm, setVolt);
            lastVolt = setVolt;
        }
    }


    void setVoltage(float value) {
        outVolt = value;
        if (value >= 0 && value < 26)
            setVolt = map(value, 1, 20, 1, 163);
    }

    void setAmperage(float value) {
        outAmps = value;
        if (value >= 0 && value <= 3)
            setAmps = map(value*100, 15, 150, 9, 94);
    }

    void setPwmVolt(uint8_t value) {
        setVolt = value;
    }

    void setPwmAmps(uint8_t value) {
        setAmps = value;
    }

    float getOutVolt() {
        return outVolt;
    }

    float getOutAmps() {
        return outAmps;
    }


    float getSetVolt() {
        return setVolt;
    }

    float getSetAmps() {
        return setAmps;
    }


    void power(boolean state) {
        isPowered = state;
    }

};
/*
 * For testing by averaging value of pwm
 *
 *
#define avrGap 10

uint8_t capMin, capMax, capAvr, capIndex = 0;
uint8_t capContainer[100];

void setToAvr(uint8_t val) {
    capContainer[capIndex] = val;
    capIndex++;

    if (capIndex > 100) {
        capIndex = 0;
    }
}


// Print array elements greater than average
void parseAvg(int arr[], int n) {
    // Find average
    double avg = 0;
    for (int i = 0; i < n; i++)
        avg += arr[i];
    avg = avg / n;
    capAvr = int(avg);

    // Print elements greater than average
    for (int i = 0; i < n; i++)
        if (arr[i] >= avg - avrGap && arr[i] <= avg + avrGap) {
            capMin = capMin > arr[i] ? arr[i] : capMin;
            capMax = capMax < arr[i] ? arr[i] : capMax;
        }
}*/

#endif //POWERSUPPLY_FUNCTIONS_H






