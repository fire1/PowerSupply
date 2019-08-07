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

#include <AnalogButtons.h>

#ifndef ANALOGBUTTONS_H_

#include "../../libraries/Analog_Buttons/AnalogButtons.h"

#endif

#include "Controller.h"


uint8_t currentButton = 0;


void btn1Click() {
    if (currentButton != 1) {
        currentButton = 1;
        Serial.println(F("button 1 clicked"));
    }
}

void btn1Hold() {
//    if (currentButton != 11) {
//        currentButton = 11;
//        Serial.println(F("button 1 hold"));
//    }
}

void btn2Click() {
    if (currentButton != 2) {
        currentButton = 2;
        Serial.println(F("button 2 clicked"));
    }
}

void btn2Hold() {
    if (currentButton != 22) {
        currentButton = 22;
        Serial.println(F("button 2 held"));
    }
}

void btn3Click() {
    if (currentButton != 3) {
        currentButton = 3;
        Serial.println(F("button 3 clicked"));
    }
}

void btn3Hold() {
    if (currentButton != 33) {
        currentButton = 33;
        Serial.println(F("button 3 held"));
    }
}

void btn4Click() {
    if (currentButton != 4) {
        currentButton = 4;
        Serial.println(F("button 4 clicked"));
    }
}

void btn4Hold() {
    if (currentButton != 44) {
        currentButton = 44;
        Serial.println(F("button 4 held"));
    }
}

void btn5Click() {
    if (currentButton != 5) {
        currentButton = 5;
        Serial.println(F("button 5 clicked"));
    }
}

void btn5Hold() {
    if (currentButton != 55) {
        currentButton = 55;
        Serial.println(F("button 5 held"));
    }
}

void btn6Click() {
    if (currentButton != 6) {
        currentButton = 6;
        Serial.println(F("button 5 clicked"));
    }
}

void btn6Hold() {
    if (currentButton != 66) {
        currentButton = 66;
        Serial.println(F("button 5 held"));
    }
}


#ifndef LiquidCrystal_h

#include "../../libraries/LiquidCrystal/src/LiquidCrystal.h"

#endif


Button btnBlinker = Button(27, &btn2Click, &btn2Hold);
Button btnEncoder = Button(610, &btn1Click, &btn1Hold);
Button btnMemSetA = Button(765, &btn3Click, &btn3Hold);
Button btnMemSetB = Button(512, &btn4Click, &btn4Hold);
Button btnMemSetC = Button(264, &btn5Click, &btn5Hold);
Button btnMemSetD = Button(420, &btn6Click, &btn6Hold);

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
    uint8_t lastPress = 0;
    unsigned long timeout, debounce;
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
        if (timeout == 0)
            timeout = currentMillis + 300000;
    }


    void inputs() {

        changeValues();
        if (currentMillis > timeout) {
            openEdit = false;
            editVolt = false;
            editAmps = false;
        }

        if (currentMillis > debounce) {
//            currentButton = 0;
        }

        lastPress = currentButton;

        //
        // Open menu and edit voltages
        if (currentButton == 1 && toggleSet) {
            toggleSet = false;
            openEdit = true;
            editVolt = true;
            editAmps = false;
            updateTimeout();
            debounce = currentMillis + 2200;
            currentButton = 0;
        }

        if (currentButton == 1 && !toggleSet) {
            toggleSet = true;
            openEdit = true;
            editVolt = false;
            editAmps = true;
            updateTimeout();
            debounce = currentMillis + 2200;
            currentButton = 0;
        }

        if (currentMillis > timeout && openEdit) {
            editAmps = false;
            editVolt = false;
            openEdit = false;
            lcdTitles = false;
            timeout = 0;
        }

        if (openEdit) {
            lcdTitles = true;

        }

    }

    void changeValues() {
        if (enc->getDirection() != 0) {
            timeout = currentMillis + 400000;
        }
        if (editVolt) {
            cnr->setVoltage(cnr->getTargetVolt() - enc->getDirection());
        }

        if (editAmps) {
            cnr->setAmperage(cnr->getTargetAmps() - enc->getDirection());
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
        abt->check();
    }

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
