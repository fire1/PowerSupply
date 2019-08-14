//
// Created by Angel Zaprianov on 2019-07-30.
//

#ifndef POWERSUPPLY_INTERFACE_H
#define POWERSUPPLY_INTERFACE_H

#include <Arduino.h>


#include "../PowerSupply.h"


#include "PowerController.h"
#include "InputInterface.h"


#include <LiquidCrystal.h>

#ifndef LiquidCrystal_4bit_h

#include "../libraries/NewLiquidCrystal_lib/LiquidCrystal.h"

#endif


class DisplayInterface {
private:

    boolean lcdBlinks = false;

    unsigned long timeout;
    String valChar;
    LiquidCrystal *lcd;
    PowerController *cnr;
    InputInterface *inp;


    void powerMode() {
        lcd->setCursor(19, 4);
        switch (cnr->getPowerMode()) {
            default:
            case PowerController::MODE_SWT_PW :
                lcd->write((byte) 1);
                break;
            case PowerController::MODE_SWT_LM :
                lcd->write((byte) 2);
                break;
            case PowerController::MODE_LIN_PW :
                lcd->write((byte) 0);
                break;
            case PowerController::MODE_LIN_LM :
                lcd->write((byte) 0);
                break;
        }
    }

    void drawMain() {
        boolean editVolt = inp->isEditVolt();
        boolean editAmps = inp->isEditAmps();
        boolean editCursor = inp->isEditPosition();

        if (editAmps || editVolt) {
            inp->setTitles(false);
        }

        bool lcdTitles = inp->isTitles();
        if (!lcdTitles || lcdBlinks) {
            lcd->clear();


            lcd->setCursor(1, 0);
            lcd->print(F("VOLTAGE"));

            lcd->setCursor(1, 2);
            lcd->print(F("CURRENT"));


            powerMode();

            if (editVolt && lcdBlinks) {
                lcd->setCursor(editCursor + 6, 1);
//                lcd->write(B01111110);
                lcd->cursor();
            }
            if (editAmps && lcdBlinks) {
                lcd->setCursor(editCursor + 6, 3);
//                lcd->write(B01111110);
                lcd->cursor();
            }

            if (!lcdBlinks) {
                lcd->noCursor();
            }

            valChar = F("A");
            lcd->setCursor(6, 3);
            lcd->print(cnr->getTargetAmps(), 3);
            lcd->print(valChar);

            valChar = F("V");
            lcd->setCursor(6, 1);
            lcd->print(cnr->getTargetVolt(), 2);
            lcd->print(valChar);
            lcdBlinks = !lcdBlinks;

        }

        valChar = F("V ");
        lcd->setCursor(14, 0);
        lcd->print(cnr->lcdVolt(), 2);
        lcd->print(valChar);

        valChar = F("A ");
        lcd->setCursor(14, 2);
        lcd->print(cnr->lcdAmps(), 3);
        lcd->print(valChar);

        if (!lcdTitles) {
            inp->setTitles(true);
            lcd->noCursor();
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
    DisplayInterface(LiquidCrystal &lc, PowerController &cn, InputInterface &in) : lcd(&lc), cnr(&cn), inp(&in) {}

    void draw() {
        drawMain();
    }


    void begin() {
        lcd->createChar(0, charLinear);
        lcd->createChar(1, charSwitch);
        lcd->createChar(2, charLimits);
        lcd->noAutoscroll();
    }


    void debug() {
        Serial.println();
        Serial.print(F("Amp In: "));
        Serial.print(cnr->lcdAmps());
        Serial.print(F(" Raw "));
        Serial.print(cnr->getDumpAmps());
        Serial.print(F(" T: "));
        Serial.print(cnr->getTargetAmps());
        Serial.print(F(" Test Amps "));
        Serial.print(cnr->testAmperage());

        Serial.print(F(" //  Volt In: "));
        Serial.print(cnr->getLiveVolt());
        Serial.print(F(" Raw "));
        Serial.print(cnr->getDumpVolt());
        Serial.print(F(" T: "));
        Serial.print(cnr->getTargetVolt());

        Serial.print(F(" PWM: "));
        Serial.print(cnr->getPwmValue());

        Serial.print(F(" ENC: "));
        Serial.print(F(" / "));


        Serial.print(F(" TMP: "));
        Serial.print(heat);

        if (inp->isEditAmps()) {
            Serial.print(F(" -> Edit AMP"));
        }

        if (inp->isEditAmps()) {
            Serial.print(F(" -> Edit VLT"));
        }
    }

};


#endif //POWERSUPPLY_INTERFACE_H
