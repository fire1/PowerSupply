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

    uint8_t cursor = 0;


    unsigned long timeout;
    PowerController *pc;
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
                Serial.print(pc->getSetVolt());
                Serial.println();
            }

            if (where == F("a")) {
                analogWrite(pinAmpPwm, Serial.readStringUntil('\n').toInt());
                Serial.println();
                Serial.print(F("CURRENT: "));
                Serial.print(pc->getSetAmps());
                Serial.println();
            }

            if (where == F("volt")) {
                pc->setVoltage(Serial.readStringUntil('\n').toFloat());
                Serial.println();
                Serial.print(F("VOLTAGE: "));
                Serial.print(pc->getSetVolt());
                Serial.println();
            }

            if (where == F("amp")) {
                pc->setAmperage(Serial.readStringUntil('\n').toFloat());
                Serial.println();
                Serial.print(F("CURRENT: "));
                Serial.print(pc->getSetAmps());
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
            if (pc->menu.editAmps) {
                pc->setAmperage(changeValue(pc->getSetAmps(), direction, !pc->menu.editHalf ?: 0.050));
                tick();
            }

            if (pc->menu.editVolt) {
                pc->setVoltage(changeValue(pc->getSetVolt(), direction, !pc->menu.editHalf ?: 0.10));
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

            case 2:
                if (edit) {
                    pc->mode.dynamic = (bool) !pc->mode.dynamic;
                } else {
                    pc->mode.powered = (bool) !pc->mode.powered;
                }
                tick();
                break;
            default:
                return;
        }
        currentButton = 0;

    }


public:
    boolean edit = false;

    InputInterface(PowerController &cn, RotaryEncoder &ec, AnalogButtons &bt, PresetMemory &pm)
            : pc(&cn), enc(&ec), ab(&bt), prm(&pm) {}

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
                edit = false;
            }
        }

    }


    uint8_t getCursor() {
        return cursor;
    }




};

#endif //POWERSUPPLY_INPUTINTERFACE_H
