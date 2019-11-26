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


////////////////////////////////////
//// Icons
const byte iconLock = 0;
byte charLock[] = {
        0b01110,
        0b10001,
        0b10001,
        0b11111,
        0b11011,
        0b11011,
        0b11111,
        0b00000
};
const byte iconUnlock = 1;
byte charUnlock[] = {
        0b01110,
        0b10000,
        0b10000,
        0b11111,
        0b11011,
        0b11011,
        0b11111,
        0b00000
};
const byte iconPlug = 2;
byte charPlugin[] = {
        B01010,
        B01010,
        B11111,
        B10001,
        B01110,
        B00100,
        B00100,
        B00000
};


const byte iconHart = 3;
byte charHart[] = {
        B01010,
        B11111,
        B11111,
        B11111,
        B01110,
        B00100,
        B00000,
        B00000
};
const byte iconSkull = 4;
byte charSkull[] = {
        B01110,
        B10101,
        B10101,
        B11011,
        B01110,
        B01110,
        B00000,
        B00000
};
const byte iconLamp = 5;
byte charLamp[] = {
        B01110,
        B10001,
        B10101,
        B10001,
        B01110,
        B01110,
        B00100,
        B00000
};
const byte iconBar2 = 6;
byte charBar2[] = {
        B00000,
        B11111,
        B00000,
        B00000,
        B00000,
        B00000,
        B11111,
        B00000
};
const byte iconBar3 = 7;
byte charBar3[] = {
        B00000,
        B00011,
        B00100,
        B00100,
        B00100,
        B00100,
        B00011,
        B00000
};


class DisplayInterface {
private:


    boolean lcdBlinks = false;
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
        drawMemory();

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
            lcd->write(pc->mode.dynamic ? iconUnlock : iconLock);
/*

            lcd->setCursor(6, 3);
            lcd->print(F("M1"));


            lcd->setCursor(10, 3);
            lcd->print(F("M2"));


            lcd->setCursor(14, 3);
            lcd->print(F("M3"));


            lcd->setCursor(18, 3);
            lcd->print(F("M4"));
*/

            lcdBlinks = !lcdBlinks;
        }


    }

    void drawMemory() {
        uint8_t mem = in->getLoaded();
        if (mem > 0) {
            drawMemCursor(mem, 126);
        }

        uint8_t set = in->getSaved();
        if (set > 0) {
            drawMemCursor(set, iconHart);
        }

    }

    void drawMemCursor(uint8_t index, byte icon) {
        switch (index) {
            case 1:
                lcd->setCursor(5, 3);
                break;
            case 2:
                lcd->setCursor(9, 3);
                break;
            case 3:
                lcd->setCursor(13, 3);
                break;

            case 4:
                lcd->setCursor(17, 3);
                break;

            default:
                break;
        }

        if (!in->edit) {
            lcd->write(icon);
            lcd->print(F("M"));
            lcd->print(index);
            in->edit = true;
        } else {
            in->closeMem();
            in->edit = false;
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
    DisplayInterface(LiquidCrystal &lc, PowerController &cn, InputInterface &in) : lcd(&lc), pc(&cn), in(&in) {}

    void draw() {
        //
        // Memory UI
        drawMain();


        lcd->setCursor(6, 1);
        voltFloat(pc->getOutVolt(), printValues);
        lcd->print(printValues);
        lcd->print(charV);


        lcd->setCursor(13, 1);
        ampsFloat(pc->getOutAmps(), printValues);
        lcd->print(printValues);
        lcd->print(charA);

        //
        // Draw bar
        // symbols: https://learn.robotgeek.com/getting-started/59-lcd-special-characters.html
        int8_t limit = pc->readLimit();
        Serial.print(F(" LN "));
        Serial.print(limit);
        for (index = 0; index < 20; ++index) {
            lcd->setCursor(index, 2);

            if (index <= limit) lcd->write("=")/*lcd->print("-")*/;
            else lcd->print("-");
        }

        //
        // Protection mode
        lcd->setCursor(3, 3);
        pc->mode.protect ? lcd->write(iconHart) : lcd->write(iconSkull);


    }


    void begin() {
        lcd->createChar(iconLock, charLock);
        lcd->createChar(iconUnlock, charUnlock);
        lcd->createChar(iconPlug, charPlugin);
        lcd->createChar(iconHart, charHart);
        lcd->createChar(iconSkull, charSkull);
        lcd->createChar(iconLamp, charLamp);
        lcd->noAutoscroll();
        delay(100);
        lcd->clear();
        lcd->home();
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
        Serial.print(F(" MD "));
        Serial.print(pc->mode.dynamic);
    }

};


#endif //POWERSUPPLY_INTERFACE_H
