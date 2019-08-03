//
// Created by Angel Zaprianov on 2019-07-30.
//

#ifndef POWERSUPPLY_INTERFACE_H
#define POWERSUPPLY_INTERFACE_H

#include <Arduino.h>
#include <EnableInterrupt.h>

#ifndef EnableInterrupt_h

#include "../../libraries/EnableInterrupt/EnableInterrupt.h"

#endif

#include <RotaryEncoder.h>

#ifndef RotaryEncoder_h

#include "../../libraries/RotaryEncoder/RotaryEncoder.h"

#endif

#include "../PowerSupply.h"
#include "Controller.h"
#include "AnalogButtons.h"

#ifndef LiquidCrystal_h

#include "../../libraries/LiquidCrystal/src/LiquidCrystal.h"

#endif


void static interruptFunction();

class UserInterface {
private:
    boolean lcdTitles = false;
    boolean lcdBlinks = false;
    boolean editVolt = false;
    boolean editAmps = false;
    boolean openEdit = false;
    boolean powerMode = true;
    boolean toggleSet = false;
    unsigned long timeout;
    String valChar;
    LiquidCrystal *lcd;
    Controller *cnr;
    RotaryEncoder *enc;
    AnalogButtons *abt;


/**
 *
 * @param value
 * @param output
 * @return
 */
    char displayVolt(float value, char *output) {

        if (value < -99) {
            value = -99;
        }

        int dig1 = int(value) * 10; // 210
        int dig2 = int((value * 10) - dig1);

        dig1 = dig1 / 10;
        if (dig2 < 0) {
            dig2 = dig2 * -1;
        }

        sprintf(output, "%02d.%02d", dig1, dig2);
    }

    /**
     *
     * @param value
     * @param output
     * @return
     */
    char displayAmps(double value, char *output) {

        if (value < -99) {
            value = -99;
        }

        int dig1 = int(value) * 100; // 210
        int dig2 = int((value * 1000) - dig1);

        dig1 = dig1 / 100;
        if (dig2 < 0) {
            dig2 = dig2 * -1;
        }

        sprintf(output, "%01d.%03d", dig1, dig2);
    }


    void drawMain() {
        if (!lcdTitles) {

            if (editVolt && lcdBlinks) {
                valChar = F(">");
                lcd->setCursor(0, 1);
                lcd->print(valChar);

            }
            lcd->clear();
            lcd->setCursor(1, 0);
            lcd->print(F("VOLTAGE"));

            lcd->setCursor(1, 2);
            lcd->print(F("CURRENT"));

            lcd->setCursor(10, 0);
            (powerMode) ? lcd->write((byte) 1) : lcd->write((byte) 0); // or 0

            if (editAmps && lcdBlinks) {
//            valChar = F(">");
                lcd->setCursor(0, 3);
                lcd->write(B01111110);
            }

            valChar = F("A");
            lcd->setCursor(3, 3);
            displayAmps(cnr->getTargetAmps(), printValues);
            lcd->print(printValues);
            lcd->print(valChar);

            valChar = F("V");
            lcd->setCursor(3, 1);
            displayVolt(cnr->getTargetVolt(), printValues);
            lcd->print(printValues);
            lcd->print(valChar);

        }

        valChar = F("V");
        lcd->setCursor(14, 0);
        displayVolt(cnr->lcdVolt(), printValues);
        lcd->print(printValues);
        lcd->print(valChar);


        valChar = F("A");
        lcd->setCursor(14, 2);
        displayAmps(cnr->lcdAmps(), printValues);
        lcd->print(printValues);
        lcd->print(valChar);

        if (!lcdTitles) {
            lcdTitles = true;
        }

        lcdBlinks = !lcdBlinks;
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

    void updateTimeout() {
        timeout = currentMillis + screenRefresh * 6;
    }


    void inputs() {

        //
        // Open menu and edit voltages
        if (abt->getButton() == AnalogButtons::btnEncoder && !toggleSet) {
            toggleSet = true;
            openEdit = true;
            editVolt = true;
            editAmps = false;
            updateTimeout();
            Serial.print(F(" TOG1 "));
        }

        if (abt->getButton() == AnalogButtons::btnEncoder && toggleSet) {
            toggleSet = false;
            openEdit = true;
            editVolt = false;
            editAmps = true;
            updateTimeout();
            Serial.print(F(" TOG0 "));
        }


        if (currentMillis > timeout) {
            openEdit = false;
            editVolt = false;
            editAmps = false;
        }

        if (openEdit) {
            lcdTitles = true;

        }
        changeValues();
    }

    void changeValues() {
        if (openEdit && editVolt) {
            cnr->setVoltage(cnr->getTargetVolt() - enc->getPosition());
        }

        if (openEdit && editAmps) {
            cnr->setAmperage(cnr->getTargetAmps() - enc->getPosition());
        }
    }

public:
    /**
     *
     * @param lc
     * @param cn
     * @param ec
     * @param bt
     */
    UserInterface(LiquidCrystal &lc, Controller &cn, RotaryEncoder &ec, AnalogButtons &bt) :
            lcd(&lc), cnr(&cn), enc(&ec), abt(&bt) {}

    void draw() {
        drawMain();
    }

    void listener() {
        terminal();
        inputs();

    }

    void begin() {

        enableInterrupt(pinEncoderA, interruptFunction, CHANGE);
        enableInterrupt(pinEncoderB, interruptFunction, CHANGE);

        pinMode(pinEncoderA, INPUT_PULLUP);
        pinMode(pinEncoderB, INPUT_PULLUP);

        lcd->createChar(0, charLinear);
        lcd->createChar(1, charSwitch);
    }


    void debug() {

        Serial.println();
        Serial.print(F("Amp In: "));
        displayAmps(cnr->lcdAmps(), printValues);
        Serial.write(printValues);
        Serial.print(F(" Raw "));
        Serial.print(cnr->getDumpAmps());
        Serial.print(F(" T: "));
        Serial.print(cnr->getTargetAmps());
        Serial.print(F(" Test Amps "));
        Serial.print(cnr->testAmperage());

        Serial.print(F(" //  Volt In: "));
        displayVolt(cnr->getLiveVolt(), printValues);
        Serial.print(printValues);
        Serial.print(F(" Raw "));
        Serial.print(cnr->getDumpVolt());
        Serial.print(F(" T: "));
        Serial.print(cnr->getTargetVolt());

        Serial.print(F(" PWM: "));
        Serial.print(cnr->getPwmValue());

        Serial.print(F(" ENC: "));
        Serial.print(enc->getDirection());
        Serial.print(F(" / "));
        Serial.print(enc->getPosition());

        if(editAmps){
            Serial.print(F(" -> Edit AMP"));
        }

        if(editVolt){
            Serial.print(F(" -> Edit VLT"));
        }
    }

};


#endif //POWERSUPPLY_INTERFACE_H