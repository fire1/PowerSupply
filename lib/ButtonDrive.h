//
// Created by Angel Zaprianov on 2019-08-09.
//

#ifndef POWERSUPPLY_BUTTONDRIVE_H
#define POWERSUPPLY_BUTTONDRIVE_H

#include <Arduino.h>
#include "../PowerSupply.h"
#include <AnalogButtons.h>

#ifndef ANALOGBUTTONS_H_

#include "../../libraries/Analog_Buttons/AnalogButtons.h"

#endif

uint8_t currentButton = 0;
uint16_t holdCounter = 0;

void driveHold(uint8_t button) {
    if (currentButton != button /*&& is250()*/) {
        holdCounter++;
        currentButton = 0; // clear old  button
    }
    if (holdCounter > holdTimeout) {
        currentButton = button;
#ifdef DEBUG
        Serial.println();
        Serial.println();
        Serial.print(F(" Button "));
        Serial.print(button);
        Serial.print(" held.");
        Serial.println();
#endif
        holdCounter = 0;
    }
}


void btn1Click() {
    if (currentButton != 1) {
        currentButton = 1;
    }
}

void btn1Hold() {
    if (currentButton != 11) {
        currentButton = 11;
    }
}

void btn2Click() {
    if (currentButton != 2) {
        currentButton = 2;
    }
}

void btn2Hold() {
    if (currentButton != 22) {
        currentButton = 22;
    }
}

void btn3Click() {
    if (currentButton != 3) {
        currentButton = 3;
    }
}

void btn3Hold() {
    if (currentButton != 3) {
        currentButton = 33;
    }
}

void btn4Click() {
    if (currentButton != 4) {
        currentButton = 4;
    }
}

void btn4Hold() {
    if (currentButton != 44) {
        currentButton = 44;
    }
}

void btn5Click() {
    if (currentButton != 5) {
        currentButton = 5;
    }
}

void btn5Hold() {
    if (currentButton != 55) {
        currentButton = 6;
    }
}

void btn6Click() {
    if (currentButton != 6) {
        currentButton = 6;
    }
}

void btn6Hold() {
    if (currentButton != 66) {
        currentButton = 66;
    }
}

Button btnBlinker = Button(20, &btn2Click, &btn2Hold, 1200, 350);
Button btnEncoder = Button(227, &btn1Click, &btn1Hold, 1200, 350);
Button btnMemSetA = Button(765, &btn3Click, &btn3Hold, 1200, 450);
Button btnMemSetB = Button(512, &btn4Click, &btn4Hold, 1200, 450);
Button btnMemSetD = Button(420, &btn6Click, &btn6Hold, 1200, 450);
Button btnMemSetC = Button(264, &btn5Click, &btn5Hold, 1200, 450);
#endif //POWERSUPPLY_BUTTONDRIVE_H
