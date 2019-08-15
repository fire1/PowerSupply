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
    boolean openEdit = false;
    boolean editVolt = false;
    boolean editAmps = false;
    boolean toggleSet = false;
    boolean lcdTitles = false;
    boolean editCursor = 0;
    uint8_t lastPress = 0;
    unsigned long timeout, debounce;
    PowerController *cnr;
    AnalogButtons *abt;
    RotaryEncoder *enc;
    PresetMemory *prm;

    Preset preset;
private:

    void updateTimeout() {
        if (timeout == 0) {
            timeout = currentLoops + editTimeout;
            cnr->power(0);
        }
    }

    void terminal() {
        if (Serial.available()) {
            String where = Serial.readStringUntil('=');
            if (where == F("v")) {
                cnr->useParse(true);
                cnr->setVoltage(Serial.readStringUntil('\n').toFloat());
                Serial.println();
                Serial.print(F("VOLTAGE: "));
                Serial.print(cnr->getTargetVolt());
                Serial.println();
            }

            if (where == F("p")) {
                cnr->useParse(false);
                cnr->setPWM(Serial.readStringUntil('\n').toInt());
                Serial.println();
                Serial.print(F("PWM: "));
                Serial.print(cnr->getPwmValue());
                Serial.println();
            }

            if (where == F("a")) {
                cnr->useParse(true);
                cnr->setAmperage(Serial.readStringUntil('\n').toFloat());
                Serial.println();
                Serial.print(F("CURRENT: "));
                Serial.print(cnr->getTargetAmps());
                Serial.println();
            }
        }
    }

/**
 *
 * @param index
 */
    void setPreset(uint8_t index) {
        prm->get(index, preset);
        cnr->setAmperage(preset.amp);
        cnr->setVoltage(preset.volt);
    }

/**
 *
 * @param index
 */
    void savePreset(uint8_t index) {
        preset.amp = cnr->getTargetAmps();
        preset.volt = cnr->getTargetVolt();

        prm->set(index, preset);
    }

    void presetButtons() {
        if (currentButton == 3) setPreset(0);
        if (currentButton == 4) setPreset(1);
        if (currentButton == 5) setPreset(2);
        if (currentButton == 6) setPreset(3);

        if (currentButton == 33) savePreset(0);
        if (currentButton == 44) savePreset(1);
        if (currentButton == 55) savePreset(2);
        if (currentButton == 66) savePreset(3);
    }

/**
 * Rotary Encoder changes
 */
    void rotaryButton() {
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
            cnr->power(1);
            editAmps = false;
            editVolt = false;
            openEdit = false;
            lcdTitles = false;
            timeout = 0;
        }
    }

    void pwmModeButton() {
        if (currentButton == 2) {
            if (lcdTitles) {
                lcdTitles = false;
                cnr->power(1);
                return;
            }


            cnr->togglePwmMode();
            lcdTitles = false;
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
        // Open  edit voltages/amperage
        rotaryButton();
        presetButtons();
        pwmModeButton();

        if (openEdit) {
            lcdTitles = true;
        }

    }

    void changeValues() {
        int direction = enc->getDirection();
        if (direction != 0) {
            timeout = currentLoops + editTimeout;
        }

        //
        // Todo check here
        float mlt = 1;
        editCursor = 0;
        if (currentButton == 11 && editVolt || currentButton == 11 && editAmps) {
            mlt = 0.1;
            editCursor = 1;
        }

        if (editVolt && is80()) {
            cnr->setVoltage(cnr->getTargetVolt() - (direction * mlt));
        }

        if (editAmps && is80()) {
            cnr->setAmperage(cnr->getTargetAmps() - (direction * mlt));
        }
    }

public:

    InputInterface(PowerController &cn, RotaryEncoder &ec, AnalogButtons &bt, PresetMemory &pm)
            : cnr(&cn), enc(&ec), abt(&bt), prm(&pm) {}

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

    boolean isEditVolt() {
        return editVolt;
    }


    boolean isEditAmps() {
        return editAmps;
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
