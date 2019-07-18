//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWERSUPPLY_FUNCTIONS_H
#define POWERSUPPLY_FUNCTIONS_H

#include <Arduino.h>
#include "header.h"

void encoder() {
    uint8_t n = digitalRead(pinEncoderA);
    if ((encoderPinALast == LOW) && (n == HIGH)) {
        if (digitalRead(pinEncoderB) == LOW) {
            encoderPos--;
        } else {
            encoderPos++;
        }
        Serial.print(encoderPos);
        Serial.print("/");
    }
    encoderPinALast = n;

}

void terminal(uint8_t &container) {
    if (Serial.available()) {
        uint8_t val = (uint8_t) Serial.parseInt(); //read int or parseFloat for ..float...
        if (val > 0) {
            container = val;
            Serial.print(F("Set point is "));
            Serial.println(val);
        }
    }
}

#endif //POWERSUPPLY_FUNCTIONS_H
