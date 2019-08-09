//
// Created by Angel Zaprianov on 2019-07-31.
//

#ifndef POWERSUPPLY_INPUTINTERFACE_H
#define POWERSUPPLY_INPUTINTERFACE_H

#include <Arduino.h>
#include "../PowerSupply.h"
#include "Controller.h"
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

void static interruptFunction();

class InputInterface {
    boolean openEdit = false;
    boolean editVolt = false;
    boolean editAmps = false;
    boolean toggleSet = false;
    boolean lcdTitles = false;
    boolean editCursor = 0;
    uint8_t lastPress = 0;
    unsigned long timeout, debounce;
    Controller *cnr;
    AnalogButtons *abt;
    RotaryEncoder *enc;

private:

    void updateTimeout() {
        if (timeout == 0)
            timeout = currentLoops + editTimeout;
    }

    void terminal() {
        if (Serial.available()) {
            String where = Serial.readStringUntil('=');
            if (where == F("v")) {
                cnr->setVoltage(Serial.readStringUntil('\n').toFloat());
                Serial.println();
                Serial.print(F("VOLTAGE: "));
                Serial.print(cnr->getTargetVolt());
                Serial.println();
            }

            if (where == F("a")) {
                cnr->setAmperage(Serial.readStringUntil('\n').toFloat());
                Serial.println();
                Serial.print(F("CURRENT: "));
                Serial.print(cnr->getTargetAmps());
                Serial.println();
            }
        }
    }

    void inputs() {

        changeValues();
        if (currentLoops > timeout) {
            openEdit = false;
            editVolt = false;
            editAmps = false;
        }


        lastPress = currentButton;

        //
        // Open menu and edit voltages
        if (currentButton == 1 && !toggleSet) {
            toggleSet = true;
            openEdit = true;
            editVolt = true;
            editAmps = false;
            updateTimeout();
            currentButton = 0;
        }

        if (currentButton == 1 && toggleSet) {
            toggleSet = false;
            openEdit = true;
            editVolt = false;
            editAmps = true;
            updateTimeout();
            currentButton = 0;
        }

        if (currentLoops > timeout && openEdit) {
            editAmps = false;
            editVolt = false;
            openEdit = false;
            lcdTitles = false;
            timeout = 0;
        }

        if (openEdit) {
            lcdTitles = true;

        }

    }

    void changeValues() {
        int direction = enc->getDirection();
        if (direction != 0) {
            timeout = currentLoops + editTimeout;
        }

        float mlt = 1;
        editCursor = 0;
        if (currentButton == 11 && editVolt || currentButton == 11 && editAmps) {
            mlt = 0.1;
            editCursor = 1;
        }

        if (editVolt && is100()) {
            cnr->setVoltage(cnr->getTargetVolt() - (direction * mlt));
        }

        if (editAmps && is100()) {
            cnr->setAmperage(cnr->getTargetAmps() - (direction * mlt));
        }
    }

public:

    InputInterface(Controller &cn, RotaryEncoder &ec, AnalogButtons &bt) : cnr(&cn), enc(&ec), abt(&bt) {}

    void begin() {


        enableInterrupt(pinEncoderA, interruptFunction, CHANGE);
        enableInterrupt(pinEncoderB, interruptFunction, CHANGE);

        pinMode(pinEncoderA, INPUT_PULLUP);
        pinMode(pinEncoderB, INPUT_PULLUP);

        abt->add(btnEncoder);
        abt->add(btnBlinker);
        abt->add(btnMemSetA);
        abt->add(btnMemSetB);
        abt->add(btnMemSetC);
        abt->add(btnMemSetD);
    }

    void listen() {
        terminal();
        abt->check();
        inputs();
    }

    boolean isEditPosition() {
        return editCursor;
    }

    boolean isTitles() {
        return lcdTitles;
    }
/**
 *
 * @param state
 */
    void setTitles(boolean state) {
        lcdTitles = state;
    }

};

#endif //POWERSUPPLY_INPUTINTERFACE_H
