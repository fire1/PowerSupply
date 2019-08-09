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
    if (currentButton != button && is350()) {
        holdCounter++;
        currentButton = 0; // clear old  button
    }
    if (holdCounter > holdTimeout) {
        currentButton = button;
        Serial.println();
        Serial.print(F(" Button "));
        Serial.print(button);
        Serial.print(" held.");
        Serial.println();
        holdCounter = 0;
    }
}


void btn1Click() {
    if (currentButton != 1) {
        currentButton = 1;
        Serial.println(F("button 1 clicked"));
    }
}

void btn1Hold() {
    driveHold(11);
}

void btn2Click() {
    if (currentButton != 2) {
        currentButton = 2;
        Serial.println(F("button 2 clicked"));
    }
}

void btn2Hold() {
    driveHold(22);
}

void btn3Click() {
    if (currentButton != 3) {
        currentButton = 3;
        Serial.println(F("button 3 clicked"));
    }
}

void btn3Hold() {
    driveHold(33);
}

void btn4Click() {
    if (currentButton != 4) {
        currentButton = 4;
        Serial.println(F("button 4 clicked"));
    }
}

void btn4Hold() {
    driveHold(44);
}

void btn5Click() {
    if (currentButton != 5) {
        currentButton = 5;
        Serial.println(F("button 5 clicked"));
    }
}

void btn5Hold() {
    driveHold(55);
}

void btn6Click() {
    if (currentButton != 6) {
        currentButton = 6;
        Serial.println(F("button 5 clicked"));
    }
}

void btn6Hold() {
    driveHold(66);
}

Button btnBlinker = Button(27, &btn2Click, &btn2Hold);
Button btnEncoder = Button(610, &btn1Click, &btn1Hold);
Button btnMemSetA = Button(765, &btn3Click, &btn3Hold);
Button btnMemSetB = Button(512, &btn4Click, &btn4Hold);
Button btnMemSetC = Button(264, &btn5Click, &btn5Hold);
Button btnMemSetD = Button(420, &btn6Click, &btn6Hold);
#endif //POWERSUPPLY_BUTTONDRIVE_H
