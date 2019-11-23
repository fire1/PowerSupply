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
    PowerController *pc;
    InputInterface *in;

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
        pc->menu.editAmps = false;
        pc->menu.editVolt = false;
        pc->menu.editHalf = false;
        uint8_t cur = in->getCursor();
        if (cur > 0) {
            in->edit = true;
            fastScreen = true;
            switch (cur) {
                case 1:
                    pc->menu.editVolt = true;
                    break;
                case 2:
                    pc->menu.editVolt = true;
                    pc->menu.editHalf = true;
                    break;
                case 3:
                    pc->menu.editAmps = true;
                    pc->menu.editHalf = true;
                    break;
                case 4:
                    pc->menu.editAmps = true;
                    break;
                default:
                    break;
            }
        }
    }

    void drawMain() {
        this->editing();

        if (in->edit || !lcdBlinks) {
            lcd->clear();

            lcd->setCursor(1, 0);
            lcd->print(F("SET "));

            lcd->setCursor(1, 1);
            lcd->print(F("OUT "));


            lcd->setCursor(6, 0);
            voltFloat(pc->getSetVolt(), printValues, pc->menu.editVolt, pc->menu.editHalf);
            lcd->print(printValues);
            lcd->print(charV);

            lcd->setCursor(13, 0);
            ampsFloat(pc->getSetAmps(), printValues, pc->menu.editAmps, pc->menu.editHalf);
            lcd->print(printValues);
            lcd->print(charA);

            lcd->setCursor(1, 3);
            lcd->write(pc->mode.dynamic ? iconLock : iconUnlock);

            lcdBlinks = !lcdBlinks;
        }

        lcd->setCursor(6, 1);
        voltFloat(pc->getOutVolt(), printValues);
        lcd->print(printValues);
        lcd->print(charV);


        lcd->setCursor(13, 1);
        ampsFloat(pc->getOutAmps(), printValues);
        lcd->print(printValues);
        lcd->print(charA);

        int8_t limit = pc->readLimit();
        Serial.print(" LN ");
        Serial.print(limit);
        for (index = 0; index < 20; ++index) {
            lcd->setCursor(index, 2);
            if (index <= limit) lcd->print("-");
            else lcd->print(" ");
        }


        lcd->setCursor(3, 3);
        pc->mode.protect ? lcd->write(iconHart) : lcd->write(iconSkull);


    }

public:
    /**
     *
     * @param lc
     * @param cn
     * @param ec
     * @param bt
     */
    DisplayInterface(LiquidCrystal &lc, PowerController &cn, InputInterface &in) : lcd(&lc), pc(&cn), in(&in) {}

    void draw() {
        drawMain();
    }


    void begin() {
        lcd->createChar(iconLock, charLock);
        lcd->createChar(iconUnlock, charUnlock);
        lcd->createChar(iconPlug, charPlugin);
        lcd->createChar(iconHart, charHart);
        lcd->createChar(iconSkull, charSkull);
        lcd->noAutoscroll();
    }


    void debug() {
        Serial.println();
        Serial.print(F(" AO "));
        Serial.print(pc->getOutAmps());
        Serial.print(F(" "));
        Serial.print(pc->getPwmAmps());
        Serial.print(F(" VO "));
        Serial.print(pc->getOutVolt());
        Serial.print(F(" "));
        Serial.print(pc->getPwmVolt());

        Serial.print(F(" TP "));
        Serial.print(temperature);
        Serial.print(F(" AB "));
        Serial.print(analogRead(A1));
        Serial.print(F(" CB "));
        Serial.print(in->lastButton);
        Serial.print(F(" CR "));
        Serial.print(in->getCursor());
//        Serial.print(F(" M "));
//        Serial.print(pc->inaRaw());
    }

};


#endif //POWERSUPPLY_INTERFACE_H
