//
// Created by Angel Zaprianov on 2019-07-30.
//

#ifndef POWERSUPPLY_INTERFACE_H
#define POWERSUPPLY_INTERFACE_H

#include <Arduino.h>
#include "../PowerSupply.h"
#include "PowerController.h"
#include "InputInterface.h"


const char charV[2] = "V";
const char charA[2] = "A";

class DisplayInterface {
private:


    boolean lcdBlinks = false;
    boolean lcdEditing = false;
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


    void drawMain() {
        boolean editVolt = false;
        boolean editAmps = false;
        boolean editHalf = false;


        if (!lcdEditing || !lcdBlinks) {
            lcd->clear();

            lcd->setCursor(1, 0);
            lcd->print(F("SET "));

            lcd->setCursor(1, 1);
            lcd->print(F("OUT "));


            lcd->setCursor(6, 1);
            voltFloat(cnr->getSetVolt(), printValues, editVolt, editHalf);
            lcd->print(printValues);
            lcd->print(charV);

            lcd->setCursor(13, 1);
            ampsFloat(cnr->getSetAmps(), printValues, editAmps, editHalf);
            lcd->print(printValues);
            lcd->print(charA);

            lcdBlinks = !lcdBlinks;
        }

        lcd->setCursor(6, 0);
        voltFloat(cnr->getOutVolt(), printValues);
        lcd->print(printValues);
        lcd->print(charV);


        lcd->setCursor(13, 0);
        ampsFloat(cnr->getOutAmps(), printValues);
        lcd->print(printValues);
        lcd->print(charA);

        if (!lcdEditing) {
            inp->setEditing(true);
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
        lcd->createChar(3, charPlugin);
        lcd->noAutoscroll();
    }


    void debug() {
        Serial.println();
        Serial.print(F("Amp In: "));
        Serial.print(cnr->getOutAmps());

        Serial.print(F(" //  Volt In: "));
        Serial.print(cnr->getOutVolt());

        Serial.print(F(" TMP "));
        Serial.print(heatSwt);

        Serial.print(F(" MN "));
        Serial.print(inp->getCursor());
        Serial.print(F(" AB "));
        Serial.print(analogRead(A1));
        Serial.print(F(" Cr "));
        Serial.print(currentButton);
    }

};


#endif //POWERSUPPLY_INTERFACE_H
