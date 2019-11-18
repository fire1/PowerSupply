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

    //
    // Ina vars
    volatile uint8_t deviceNumber = UINT8_MAX; ///< Device Number to use in example, init used for detection loop
    volatile uint64_t sumBusMillVolts = 0; ///< Sum of bus voltage readings
    volatile int64_t sumBusMicroAmps = 0; ///< Sum of bus amperage readings
    volatile uint8_t readings = 0; ///< Number of measurements taken


    void setupIna() {
        uint8_t devicesFound = 0;
        while (deviceNumber == UINT8_MAX) // Loop until we find the first device
        {
            noTone(pinTone);
            devicesFound = ina.begin(3, 1000000); // Set to an expected 1 Amp maximum and a 100000 microOhm resistor
            for (uint8_t i = 0; i < devicesFound; i++) {
                // Change the "INA226" in the following statement to whatever device you have attached and want to measure //
                if (strcmp(ina.getDeviceName(i), "INA226") == 0) {
                    deviceNumber = i;
                    ina.reset(deviceNumber); // Reset device to default settings
                    break;
                } // of if-then we have found an INA226
            } // of for-next loop through all devices found
            if (deviceNumber == UINT8_MAX) {
                Serial.print(F("No ina found. Waiting 1s and retrying...\n"));
                tone(pinTone, 1600);
                delay(1000);
            } // of if-then no INA226 found
        }


        ina.setAveraging(64, deviceNumber);                  // Average each reading 64 times
        ina.setBusConversion(8244, deviceNumber);            // Maximum conversion time 8.244ms
        ina.setShuntConversion(8244, deviceNumber);          // Maximum conversion time 8.244ms
        ina.setMode(INA_MODE_CONTINUOUS_BOTH, deviceNumber); // Bus/shunt measured continuously
        ina.AlertOnConversion(true, deviceNumber);

    }

    void calculateOutput() {
        if (readings >= 10) {
            outVolt = (float) sumBusMillVolts / readings / 1000.0;
            outAmps = (float) sumBusMicroAmps / readings / 1000.0;
        }
    }


public:

    EditMenu menu;

    void begin() {

        enableInterrupt(pinInaAlert, inaAlertInterrupt, CHANGE);
        this->setupIna();
        pinMode(pinLed, OUTPUT);

        pinMode(pinVolPwm, OUTPUT);
        pinMode(pinAmpPwm, OUTPUT);
        //
        // Pin 9/10 timer setup
        TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
    }


    void manage() {
        this->calculateOutput();

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

    void measure() {
        sumBusMillVolts += ina.getBusMilliVolts(deviceNumber);    // Add current value to sum
        sumBusMicroAmps += ina.getBusMicroAmps(deviceNumber);     // Add current value to sum
        readings++;
        ina.waitForConversion(deviceNumber);
    }

////////////////////////////////////////////////////////////////
///     Class I/O [Input/Output]
////////////////////////////////////////////////////////////////

/**
 * Converts given voltage to PWM value
 * @param value
 */
    void setVoltage(float value) {
        setVolt = value;
        if (value >= 0 && value < 26) {
            pwmVolt = map(value * 10, 10, 205, 15, 167);
        }
    }

/**
 * Converts given current to PWM value
 * @param value
 */
    void setAmperage(float value) {
        setAmps = value;
        if (value >= 0 && value <= 3)
            pwmAmps = map(value * 100, 10, 200, 8, 180);
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


    void power(boolean state) {
        isPowered = state;
    }

};

#endif //POWERSUPPLY_FUNCTIONS_H






