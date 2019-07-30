//
// Created by Angel Zaprianov on 2019-07-30.
//

#ifndef POWERSUPPLY_INTERFACE_H
#define POWERSUPPLY_INTERFACE_H

#include <Arduino.h>
#include "../PowerSupply.h"
#include "Controller.h"

#ifndef LiquidCrystal_h

#include "../../libraries/LiquidCrystal/src/LiquidCrystal.h"

#endif

class Interface {
private:
    boolean lcdTitles = false;
    boolean lcdBlinks = false;
    boolean editVolt = false;
    boolean editAmps = false;
    uint8_t encoderPos = 0;
    uint8_t encoderPinALast = LOW;
    String valChar;
    LiquidCrystal *lcd;
    Controller *cnr;

    void encoder() {
        uint8_t n = digitalRead(pinEncoderA);
        if ((encoderPinALast == LOW) && (n == HIGH)) {
            if (digitalRead(pinEncoderB) == LOW) {
                encoderPos--;
            } else {
                encoderPos++;
            }
            Serial.print(encoderPos);
            Serial.print(F("/"));
        }
        encoderPinALast = n;

    }

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
        int dig2 = int((value * 100) - dig1);

        dig1 = dig1 / 100;
        if (dig2 < 0) {
            dig2 = dig2 * -1;
        }

        sprintf(output, "%01d.%03d", dig1, dig2);
    }


    void drawMain() {
        if (editVolt && lcdBlinks) {
            valChar = F(">");
            lcd->setCursor(0, 1);
            lcd->print(valChar);
        }

        valChar = F("V");

        if (!lcdTitles) {
            lcd->clear();
            lcd->setCursor(1, 0);
            lcd->print(F("VOLTAGE"));
        }

        lcd->setCursor(3, 1);
        displayVolt(cnr->getTargetVolt(), printValues);
        lcd->print(printValues);
        lcd->print(valChar);
        lcd->setCursor(9, 0);
        displayVolt(cnr->lcdVolt(), printValues);
        lcd->print(printValues);
        lcd->print(valChar);


        if (editAmps && lcdBlinks) {
            valChar = F(">");
            lcd->setCursor(0, 3);
            lcd->print(valChar);
        }

        valChar = F("A");
        if (!lcdTitles) {
            lcd->setCursor(1, 2);
            lcd->print(F("CURRENT"));
        }

        lcd->setCursor(3, 3);
        displayAmps(cnr->getTargetAmps(), printValues);
        lcd->print(printValues);
        lcd->print(valChar);
        lcd->setCursor(9, 2);
        displayAmps(cnr->lcdAmps(), printValues);
        lcd->print(printValues);
        lcd->print(valChar);

        if (!lcdTitles) {
            lcdTitles = true;
        }

        lcdBlinks = !lcdBlinks;
    }

public:
    Interface(LiquidCrystal &lc, Controller &cn) : lcd(&lc), cnr(&cn) {}

    void draw() {
        drawMain();
    }

    void begin(){
        pinMode(pinEncoderA, INPUT_PULLUP);
        pinMode(pinEncoderB, INPUT_PULLUP);
    }


    void listener() {
        if (Serial.available()) {
            String where = Serial.readStringUntil('=');
            if (where == F("v")) {
                cnr->setVoltage(Serial.readStringUntil('\n').toFloat());
            }

            if (where == F("a")) {
                cnr->setAmperage(Serial.readStringUntil('\n').toFloat());
            }
        }
        encoder();
    }

    void debug() {

        Serial.println();
        Serial.print(F("Amp In: "));
        displayAmps(cnr->getLiveAmps(), printValues);
        Serial.write(printValues);
        Serial.print(F(" Raw "));
        Serial.print(cnr->getDumpAmps());
        Serial.print(F(" T: "));
        Serial.print(cnr->getTargetAmps());

        Serial.print(F(" //  Volt In: "));
        displayVolt(cnr->getLiveVolt(), printValues);
        Serial.print(printValues);
        Serial.print(F(" Raw "));
        Serial.print(cnr->getDumpVolt());
        Serial.print(F(" T: "));
        Serial.print(cnr->getTargetVolt());

        Serial.print(F(" PWM: "));
        Serial.print(cnr->getPwmValue());

    }

};


#endif //POWERSUPPLY_INTERFACE_H
