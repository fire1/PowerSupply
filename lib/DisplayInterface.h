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
    boolean lcdTitles = false;
    boolean lcdBlinks = false;
    boolean editVolt = false;
    boolean editAmps = false;
    boolean editCursor = 0;
    unsigned long timeout;
    String valChar;
    LiquidCrystal *lcd;
    PowerController *cnr;
    InputInterface *inp;


    void drawMain() {

        if (editAmps || editVolt) {
            lcdTitles = false;
        }
        editCursor = inp->isEditPosition();
        bool lcdTitles = inp->isTitles();
        if (!lcdTitles || lcdBlinks) {
            lcd->clear();

            if (editVolt && lcdBlinks) {
                lcd->setCursor(editCursor, 1);
                lcd->write(B01111110);
            }

            lcd->setCursor(1, 0);
            lcd->print(F("VOLTAGE"));

            lcd->setCursor(1, 2);
            lcd->print(F("CURRENT"));

            lcd->setCursor(10, 0);
//            (powerMode) ? lcd->write((byte) 1) : lcd->write((byte) 0); // or 0

            if (editAmps && lcdBlinks) {
                lcd->setCursor(editCursor, 3);
                lcd->write(B01111110);
            }

            valChar = F("A");
            lcd->setCursor(3, 3);
            lcd->print(cnr->getTargetAmps(), 3);
            lcd->print(valChar);

            valChar = F("V");
            lcd->setCursor(3, 1);
            lcd->print(cnr->getTargetVolt(), 2);
            lcd->print(valChar);
            lcdBlinks = !lcdBlinks;
        }

        valChar = F("V");
        lcd->setCursor(14, 0);
        lcd->print(cnr->lcdVolt(), 2);
        lcd->print(valChar);


        valChar = F("A");
        lcd->setCursor(14, 2);
        lcd->print(cnr->lcdAmps(), 3);
        lcd->print(valChar);

        if (!lcdTitles) {
            inp->setTitles(true);
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

        if (editAmps) {
            Serial.print(F(" -> Edit AMP"));
        }

        if (editVolt) {
            Serial.print(F(" -> Edit VLT"));
        }
    }

};


#endif //POWERSUPPLY_INTERFACE_H
