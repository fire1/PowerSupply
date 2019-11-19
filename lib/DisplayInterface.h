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
    PowerController *cr;
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


    void editing() {
        cr->menu.editAmps = false;
        cr->menu.editVolt = false;
        cr->menu.editHalf = false;
        uint8_t cur = inp->getCursor();
        if (cur > 0) {
            lcdEditing = true;
            fastScreen = true;
            switch (cur) {
                case 1:
                    cr->menu.editVolt = true;
                    break;
                case 2:
                    cr->menu.editVolt = true;
                    cr->menu.editHalf = true;
                    break;
                case 3:
                    cr->menu.editAmps = true;
                    cr->menu.editHalf = true;
                    break;
                case 4:
                    cr->menu.editAmps = true;
                    break;
                default:
                    break;
            }
        }
    }

    void drawMain() {
        this->editing();

        if (lcdEditing || !lcdBlinks) {
            lcd->clear();

            lcd->setCursor(1, 0);
            lcd->print(F("SET "));

            lcd->setCursor(1, 1);
            lcd->print(F("OUT "));


            lcd->setCursor(6, 0);
            voltFloat(cr->getSetVolt(), printValues, cr->menu.editVolt, cr->menu.editHalf);
            lcd->print(printValues);
            lcd->print(charV);

            lcd->setCursor(13, 0);
            ampsFloat(cr->getSetAmps(), printValues, cr->menu.editAmps, cr->menu.editHalf);
            lcd->print(printValues);
            lcd->print(charA);

            lcdBlinks = !lcdBlinks;
        }

        lcd->setCursor(6, 1);
        voltFloat(cr->getOutVolt(), printValues);
        lcd->print(printValues);
        lcd->print(charV);


        lcd->setCursor(13, 1);
        ampsFloat(cr->getOutAmps(), printValues);
        lcd->print(printValues);
        lcd->print(charA);

        int8_t limit = cr->readLimit();
        for (index = 0; index < limit; ++index) {
            lcd->setCursor(index, 3);
            lcd->print("-");
        }


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
    DisplayInterface(LiquidCrystal &lc, PowerController &cn, InputInterface &in) : lcd(&lc), cr(&cn), inp(&in) {}

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
        Serial.print(F(" AO "));
        Serial.print(cr->getOutAmps());
        Serial.print( F(" "));
        Serial.print(cr->getPwmAmps());
        Serial.print(F(" VO "));
        Serial.print(cr->getOutVolt());
        Serial.print( F(" "));
        Serial.print(cr->getPwmVolt());

        Serial.print(F(" TP "));
        Serial.print(temperature);
        Serial.print(F(" AB "));
        Serial.print(analogRead(A1));
        Serial.print(F(" CB "));
        Serial.print(currentButton);
        Serial.print(F(" CR "));
        Serial.print(inp->getCursor());
    }

};


#endif //POWERSUPPLY_INTERFACE_H
