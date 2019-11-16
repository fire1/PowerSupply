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

void static interruptFunction();

class InputInterface {
    boolean isEdit = false;
    uint8_t cursor = 0;


    unsigned long timeout;
    PowerController *cnr;
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
                Serial.print(cnr->getSetVolt());
                Serial.println();
            }

            if (where == F("a")) {
                analogWrite(pinAmpPwm, Serial.readStringUntil('\n').toInt());
                Serial.println();
                Serial.print(F("CURRENT: "));
                Serial.print(cnr->getSetAmps());
                Serial.println();
            }

            if (where == F("volt")) {
                cnr->setVoltage(Serial.readStringUntil('\n').toFloat());
                Serial.println();
                Serial.print(F("VOLTAGE: "));
                Serial.print(cnr->getSetVolt());
                Serial.println();
            }

            if (where == F("amp")) {
                cnr->setAmperage(Serial.readStringUntil('\n').toFloat());
                Serial.println();
                Serial.print(F("CURRENT: "));
                Serial.print(cnr->getSetAmps());
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
        Serial.println();
        Serial.print(timeout);
        Serial.print(" ");
        Serial.print(millis());
    }


    void inputs() {

        int direction = enc->getDirection();
        if (direction != 0) {
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
                ping();
                break;
            default:
                return;
        }
        currentButton = 0;

    }


public:

    InputInterface(PowerController &cn, RotaryEncoder &ec, AnalogButtons &bt, PresetMemory &pm)
            : cnr(&cn), enc(&ec), ab(&bt), prm(&pm) {}

    void begin() {


        enableInterrupt(pinEncoderA, interruptFunction, CHANGE);
        enableInterrupt(pinEncoderB, interruptFunction, CHANGE);

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
        terminal();
        ab->check();
        buttons();

        if (cursor > 0 && timeout <= millis()) {
            cursor = 0;
            Serial.println(" timeout ");
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
