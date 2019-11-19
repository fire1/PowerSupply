//
// Created by Angel Zaprianov on 2019-07-31.
//

#ifndef POWERSUPPLY_INPUTINTERFACE_H
#define POWERSUPPLY_INPUTINTERFACE_H

#include <Arduino.h>
#include "../PowerSupply.h"
#include "PowerController.h"
#include <EnableInterrupt.h>

#ifndef EnableInterrupt_h

#include "../../libraries/EnableInterrupt/EnableInterrupt.h"

#endif

#include <RotaryEncoder.h>

#ifndef RotaryEncoder_h

#include "../../libraries/RotaryEncoder/RotaryEncoder.h"

#endif

#include <AnalogButtons.h>

#ifndef ANALOGBUTTONS_H_

#include "../../libraries/Analog_Buttons/AnalogButtons.h"

#endif

#include "ButtonDrive.h"
#include "PresetMemory.h"

void static encoderInterrupt();

class InputInterface {
    boolean isEdit = false;
    uint8_t cursor = 0;


    unsigned long timeout;
    PowerController *cr;
    AnalogButtons *ab;
    RotaryEncoder *enc;
    PresetMemory *prm;
    Preset preset;
private:


    void terminal() {
        if (Serial.available()) {
            String where = Serial.readStringUntil('=');

            if (where == F("v")) {
                analogWrite(pinVolPwm, Serial.readStringUntil('\n').toInt());
                Serial.println();
                Serial.print(F("VOLTAGE: "));
                Serial.print(cr->getSetVolt());
                Serial.println();
            }

            if (where == F("a")) {
                analogWrite(pinAmpPwm, Serial.readStringUntil('\n').toInt());
                Serial.println();
                Serial.print(F("CURRENT: "));
                Serial.print(cr->getSetAmps());
                Serial.println();
            }

            if (where == F("volt")) {
                cr->setVoltage(Serial.readStringUntil('\n').toFloat());
                Serial.println();
                Serial.print(F("VOLTAGE: "));
                Serial.print(cr->getSetVolt());
                Serial.println();
            }

            if (where == F("amp")) {
                cr->setAmperage(Serial.readStringUntil('\n').toFloat());
                Serial.println();
                Serial.print(F("CURRENT: "));
                Serial.print(cr->getSetAmps());
                Serial.println();
            }


            if (where == F("fan")) {
                int pwm = Serial.readStringUntil('\n').toInt();
                analogWrite(pinFans, pwm);
                Serial.println();
                Serial.print(F("FAn: "));
                Serial.print(pwm);
                Serial.println();
            }
        }
    }

    void ping() {
        timeout = millis() + editTimeout;
    }


    float changeValue(float value, int8_t direction, double rate = 1) {
        return (direction > 0) ? value + rate : value - rate;
    }

    void input() {

        int direction = enc->getDirection();
        if (direction != 0) {
            if (cr->menu.editAmps) {
                cr->setAmperage(changeValue(cr->getSetAmps(), direction, !cr->menu.editHalf ?: 0.050));
                tick();
            }

            if (cr->menu.editVolt) {
                cr->setVoltage(changeValue(cr->getSetVolt(), direction, !cr->menu.editHalf ?: 0.10));
                tick();
            }

            ping();
        }

    }


    void buttons() {
        switch (currentButton) {
            case 1:
                cursor++;
                if (cursor > 4) {
                    cursor = 1;
                }
                tick();
                ping();
                break;
            default:
                return;
        }
        currentButton = 0;

    }


public:

    InputInterface(PowerController &cn, RotaryEncoder &ec, AnalogButtons &bt, PresetMemory &pm)
            : cr(&cn), enc(&ec), ab(&bt), prm(&pm) {}

    void begin() {


        enableInterrupt(pinEncoderA, encoderInterrupt, CHANGE);
        enableInterrupt(pinEncoderB, encoderInterrupt, CHANGE);

        pinMode(pinEncoderA, INPUT_PULLUP);
        pinMode(pinEncoderB, INPUT_PULLUP);
        pinMode(pinAnalogBt, INPUT_PULLUP);

        ab->add(btnEncoder);
        ab->add(btnBlinker);
        ab->add(btnMemSetA);
        ab->add(btnMemSetB);
        ab->add(btnMemSetC);
        ab->add(btnMemSetD);
    }

    void listen() {
        this->terminal();
        ab->check();
        this->buttons();

        if (cursor > 0) {
            this->input();
            if (timeout <= millis()) {
                cursor = 0;
                Serial.println(F(" timeout "));
            }
        }

    }


    uint8_t getCursor() {
        return cursor;
    }


/**
 *
 * @param state
 */
    void setEditing(boolean state) {
        isEdit = state;
    }


};

#endif //POWERSUPPLY_INPUTINTERFACE_H
