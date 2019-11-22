//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWER_SUPPLY_FUNCTIONS_H
#define POWER_SUPPLY_FUNCTIONS_H

#include <Arduino.h>
#include <avr/io.h>
#include "../PowerSupply.h"

#include <EnableInterrupt.h>

#ifndef EnableInterrupt_h

#include "../../libraries/EnableInterrupt/EnableInterrupt.h"

#endif

#include <INA.h>

#ifndef INA__Class_h

#include "../../libraries/INA2xx/src/INA.h"

#endif

INA_Class ina;


/*#include <INA226.h>

#ifndef INA226_h

#include "../../libraries/Arduino-INA226/INA226.h"

#endif

INA226 ina;*/

void static inaAlertInterrupt();


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
    uint8_t pwmAmps = 21 /*8*/, lastAmps;
    float setVolt = 3.0;
    float setAmps = 0.200;
    float outVolt = 0;
    float outAmps = 0;

    //
    // Ina vars
    volatile uint8_t deviceNumber = UINT8_MAX; ///< Device Number to use in example, init used for detection loop
    volatile uint64_t sumBusMillVolts = 0; ///< Sum of bus voltage readings
    volatile int64_t sumBusMicroAmps = 0; ///< Sum of bus amperage readings
    volatile uint8_t readings = 0; ///< Number of measurements taken




public:

    EditMenu menu;

    void begin() {

        enableInterrupt(pinInaAlert, inaAlertInterrupt, CHANGE);
//        this->setupIna();
        pinMode(pinLed, OUTPUT);
        // Display configuration


        pinMode(pinVolPwm, OUTPUT);
        pinMode(pinAmpPwm, OUTPUT);
        pinMode(pinAmpLimit, INPUT_PULLUP);
        digitalWrite(pinVolPwm, LOW);
        digitalWrite(pinAmpPwm, HIGH);
        //
        // Pin 9/10 timer setup
        TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
//        TCCR1B = TCCR1B & B11111000 | B00000001; // set timer 1 divisor to 1 for PWM frequency of 31372.55 Hz
    }


    void calculate() {

/*        outVolt = (float) ina.readBusVoltage();
        outAmps = (float) ina.readShuntCurrent();*/

        outVolt = (float) ina.getBusMilliVolts() / 1000.0;
        outAmps = (float) ina.getBusMicroAmps() / 1000000.0;


        Serial.println("  ");

        Serial.println("");
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


////////////////////////////////////////////////////////////////
///     Class I/O [Input/Output]
////////////////////////////////////////////////////////////////

/**
 * Converts given voltage to PWM value
 * @param value
 */
    void setVoltage(float value) {
        if (value > 26) {
            value = 26;
        }
        if (value >= 0 && value <= 26) {
            setVolt = value;
            if (value == 26) {
                pwmVolt = 255;
            }
            pwmVolt = map(value * 10, 10, 205, 15, 167);
        }
    }

/**
 * Converts given current to PWM value
 * @param value
 */
    void setAmperage(float value) {
        if (value > 3) {
            value = 3;
        }
        if (value >= 0 && value <= 3) {
            setAmps = value;
//        pwmAmps = map(value * 100, 10, 200, 20/*8*/, 180);
            pwmAmps = map(value * 100, 20, 300, 20/*8*/, 255);
        }
    }

/**
 * Sets raw PWM voltage
 * @param value
 */
    void setPwmVolt(uint8_t value) {
        pwmVolt = value;
    }

/**
 * Sets raw PWM current
 * @param value
 */
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

    uint8_t cntFix = 0;

/**
 *  Current limit
 * @return 0,20
 */
    int8_t readLimit() {
        int limit = analogRead(pinAmpLimit);
//        if (limit < 100) {
//            analogWrite(pinVolPwm, LOW);
//            digitalWrite(pinAmpPwm,HIGH);
//            delayMicroseconds(100);
//            digitalWrite(pinAmpPwm,pwmAmps);
//            analogWrite(pinVolPwm, pwmVolt);
//        }
        Serial.print(" L ");
        Serial.print(limit);
        if (limit < 200) {
            digitalWrite(pinLed, HIGH);

        }
        return (int8_t) map(limit, 0, 1024, 20, 0);
    }


    void power(boolean state) {
        isPowered = state;
    }

};

#endif //POWERSUPPLY_FUNCTIONS_H






