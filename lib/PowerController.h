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

void static inaAlertInterrupt();

void static ampLimitInterrupt();


#ifndef CONTROL_INTERVAL
#define CONTROL_INTERVAL 300
#endif

#ifndef CONTROL_TOLERANCE
#define CONTROL_TOLERANCE 3
#endif


struct EditMenu {
    boolean editVolt = false;
    boolean editAmps = false;
    boolean editHalf = false;
};

struct PowerMode {
    boolean powered = true;
    boolean dynamic = false;
    boolean protect = true;
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
    boolean charging = false;
    uint8_t pwmVolt = 31, lastVolt, ctrVolt;
    uint8_t pwmAmps = 19, lastAmps, ctrAmps;
    uint8_t lastLimit = 0;
    float setVolt = 3.0;
    float setAmps = 0.200;
    float outVolt = 0;
    float outAmps = 0;
    unsigned long lastControl = 0;


    void controller() {
        unsigned long interval = millis();
        if (interval > lastControl + CONTROL_INTERVAL) {
            if (outVolt > setVolt && outAmps == 0) {
                pwmVolt--;
                pwmVolt = constrain(pwmVolt, ctrVolt - CONTROL_TOLERANCE, ctrVolt + CONTROL_TOLERANCE);
            }

            if (outVolt < setVolt && outAmps == 0) {
                pwmVolt++;
                pwmVolt = constrain(pwmVolt, ctrVolt - CONTROL_TOLERANCE, ctrVolt + CONTROL_TOLERANCE);
            }

/*        if (outAmps > setAmps && controlInterval()) {
            pwmAmps--;
            pwmAmps = constrain(pwmAmps, 10, 150);
        }*/

            lastControl = interval;
        }
    }

    void charge() {
        if (outAmps < 0.10 || outVolt >= setVolt) {
            charging = false;
            mode.powered = false;
            alarm();
        } else {
            mode.powered = true;
            charging = true;
        }
    }

public:

    PowerMode mode;
    EditMenu menu;
    boolean isBat = false;

    void begin() {

        pinMode(pinLed, OUTPUT);
        pinMode(pinVolPwm, OUTPUT);
        pinMode(pinAmpPwm, OUTPUT);
        pinMode(pinAmpLimit, INPUT_PULLUP);
        digitalWrite(pinVolPwm, LOW);
        digitalWrite(pinAmpPwm, HIGH);


        enableInterrupt(pinInaAlert, inaAlertInterrupt, CHANGE);
        enableInterrupt(pinAmpLimit, ampLimitInterrupt, CHANGE);
        ampLimitInterrupt();
        //
        // Pin 9/10 timer setup
        TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
//        TCCR1B = TCCR1B & B11111000 | B00000001; // set timer 1 divisor to 1 for PWM frequency of 31372.55 Hz
    }


    void calculate() {
        outVolt = (float) ina.getBusMilliVolts() / 1000.0;
        outAmps = (float) ina.getBusMicroAmps() / 1000000.0;
    }


    void manage() {
        if (mode.protect && outVolt < 0.5 && outAmps > 0.25) {
            mode.powered = false;
            alarm();
        }

        if (mode.dynamic) {
            controller();
        }

        if (isBat) {
            charge();
        }

        if (!mode.powered) {
            lastVolt = 0;
            analogWrite(pinVolPwm, 0);
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


    /**
 *  Current limit
 * @return 0,20
 */
    int8_t readLimit() {
        if (mode.protect && outVolt < 1) {
            blink();
        }

        int limit = analogRead(pinAmpLimit);
        Serial.print(" L ");
        Serial.print(limit);
        if (limit < 200) {
            digitalWrite(pinLed, HIGH);
        }
        return (int8_t) map(limit, 50, 17, 0, 20);
    }

/**
 *
 * @param ui
 */
    boolean isLimited() {
        if (mode.protect && outVolt < 1) {
            blink();
        }

        return lastLimit != ampLimiter;
    }

    int8_t getLimit() {


        return (int8_t) map(ampLimiter, 50, 17, 0, 20);
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
            ctrVolt = pwmVolt;
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
            pwmAmps = map(value * 100, 20, 300, 19, 255);
            ctrAmps = pwmAmps;
        }
    }

/**
 * Sets raw PWM voltage
 * @param value
 */
    void setPwmVolt(uint8_t value) {
        setVolt = (float) map(value, 15, 167, 10, 205) * 0.1;
        pwmVolt = value;
    }

/**
 * Sets raw PWM current
 * @param value
 */
    void setPwmAmps(uint8_t value) {
        setAmps = (float) map(value, 19, 255, 20, 300) * 0.01;
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

    PowerMode getMode() {
        return mode;
    }


};

#endif //POWERSUPPLY_FUNCTIONS_H






