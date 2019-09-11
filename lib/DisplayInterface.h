//
// Created by Angel Zaprianov on 2019-07-30.
//

#ifndef POWERSUPPLY_INTERFACE_H
#define POWERSUPPLY_INTERFACE_H

#include <Arduino.h>


#include "../PowerSupply.h"


#include "PowerController.h"
#include "InputInterface.h"

#ifndef DEBUG

#include <LiquidCrystal.h>

#ifndef LiquidCrystal_4bit_h

#include "../libraries/NewLiquidCrystal_lib/LiquidCrystal.h"

#endif
#endif

const char charV[2] = "V";
const char charA[2] = "A";

class DisplayInterface {
private:


    boolean lcdBlinks = false;

    unsigned long timeout;
    LiquidCrystal *lcd;
    PowerController *cnr;
    InputInterface *inp;

/**
 *
 * @param value
 * @param output
 * @param edit
 * @param half
 */
    void voltFloat(float value, char *output, boolean edit = false, boolean half = false) {

        if (value < -99) {
            value = -99;
        }

        int dig1 = int(value); // 210
        int dig2 = int(int(value * 1000) - dig1 * 1000);

        if (dig2 < 0) {
            dig2 = dig2 * -1;
        }

        dig2 /= 10;


        if (edit && half && lcdBlinks) {
            sprintf(output, "%02d.  ", dig1);
        } else if (edit && lcdBlinks) {
            sprintf(output, "  .%02d", dig2);
        } else {
            sprintf(output, "%02d.%02d", dig1, dig2);
        }
    }

/**
 *
 * @param value
 * @param output
 * @param edit
 * @param half
 */
    void ampsFloat(float value, char *output, boolean edit = false, boolean half = false) {

        if (value < -99) {
            value = -99;
        }

        int dig1 = int(value); // 210
        int dig2 = int(int(value * 1000) - dig1 * 1000);

        if (dig2 < 0) {
            dig2 = dig2 * -1;
        }

        if (edit && half && lcdBlinks) {
            sprintf(output, "%01d.   ", dig1);
        } else if (edit && lcdBlinks) {
            sprintf(output, " .%03d", dig2);
        } else {
            sprintf(output, "%01d.%03d", dig1, dig2);
        }

    }


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
        uint8_t presetSet = inp->getSavedPreset();
        uint8_t usedSet = inp->getPresetUsed();


        if (editAmps || editVolt || presetSet != 0 || usedSet != 0) {
            inp->setTitles(false);
        }

        bool lcdTitles = inp->isTitles();
        if (!lcdTitles || !lcdBlinks) {
            lcd->clear();

            lcd->setCursor(1, 0);
            lcd->print(F("VOLTAGE"));

            lcd->setCursor(1, 2);
            lcd->print(F("CURRENT"));


            powerMode();

            lcd->setCursor(5, 1);
            voltFloat(cnr->getTargetVolt(), printValues, editVolt, editCursor);
            lcd->print(printValues);
            lcd->print(charV);

            lcd->setCursor(5, 3);
            ampsFloat(cnr->getTargetAmps(), printValues, editAmps, editCursor);
            lcd->print(printValues);
            lcd->print(charA);

            lcdBlinks = !lcdBlinks;

            if (presetSet > 0) {
                lcd->setCursor(0, 3);
                lcd->print((">"));
                lcd->print(presetSet);
            }
            if (usedSet > 0) {
                lcd->setCursor(0, 3);
                lcd->print(usedSet);
                lcd->print((">"));
                lcd->print((">"));
            }

        }

        lcd->setCursor(14, 0);
        voltFloat(cnr->lcdVolt(), printValues);
        lcd->print(printValues);
        lcd->print(charV);


        lcd->setCursor(14, 2);
        ampsFloat(cnr->lcdAmps(), printValues);
        lcd->print(printValues);
        lcd->print(charA);

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

        Serial.print(F(" TMP S/L: "));
        Serial.print(heatSwt);
        Serial.print(F(" / "));
        Serial.print(heatLin);

        if (inp->isEditAmps()) {
            Serial.print(F(" -> Edit AMP"));
        }

        if (inp->isEditAmps()) {
            Serial.print(F(" -> Edit VLT"));
        }
    }

};


#endif //POWERSUPPLY_INTERFACE_H
