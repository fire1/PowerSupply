//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWER_SUPPLY_FUNCTIONS_H
#define POWER_SUPPLY_FUNCTIONS_H

#include <Arduino.h>
#include "../PowerSupply.h"


ResponsiveAnalogRead rawVolt(pinVolInp, true);
ResponsiveAnalogRead rawAmps(pinAmpInp, true);

struct EditMenu {
    boolean editVolt = false;
    boolean editAmps = false;
    boolean editHalf = false;
};


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
// step 8.2

class PowerController {

    boolean isPowered = true;
    uint8_t pwmVolt = 31, lastVolt;
    uint8_t pwmAmps = 8, lastAmps;
    float setVolt = 3.0;
    float setAmps = 0.100;
    float outVolt = 0;
    float outAmps = 0;


    void setupPwm() {

    }

public:

    EditMenu menu;

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
        if (lastAmps != pwmAmps) {
            analogWrite(pinAmpPwm, pwmAmps);
            lastAmps = pwmAmps;
        }
        if (lastVolt != pwmVolt) {
            analogWrite(pinVolPwm, pwmVolt);
            lastVolt = pwmVolt;
        }
    }


    void setVoltage(float value) {
        setVolt = value;
        if (value >= 0 && value < 26) {
            pwmVolt = map(value * 10, 10, 205, 15, 167);
        }
    }

    void setAmperage(float value) {
        setAmps = value;
        if (value >= 0 && value <= 3)
            pwmAmps = map(value * 100, 10, 200, 8, 180);
    }

    void setPwmVolt(uint8_t value) {
        pwmVolt = value;
    }

    void setPwmAmps(uint8_t value) {
        pwmAmps = value;
    }

    uint8_t getPwmAmps() {
        return pwmAmps;
    }

    uint8_t getPwmVolt() {
        return pwmVolt;
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

#endif //POWERSUPPLY_FUNCTIONS_H






