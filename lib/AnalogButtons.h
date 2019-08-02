//
// Created by Admin on 8/2/2019.
//

#ifndef POWERSUPPLY_ANALOGBUTTONS_H
#define POWERSUPPLY_ANALOGBUTTONS_H

#include <Arduino.h>
#include <PowerSupply.h>

typedef static uint8_t btn;

class AnalogButtons {
    btn now = 0;
    int read;


private:

    void encodeClick() {

        if (read > 550 && read < 650) {
            now = btnEncoder;
        }
    }

public:

    const btn btnNone = 0;
    const btn btnEncoder = 1;

    void setup() {
        pinMode(pinAnalogBt, INPUT_PULLUP);
    }

    void sens() {
        read = analogRead(pinAnalogBt);
    }

    btn getbutton() {
        btn cp = now;
        now = btnNone;
        return cp;
    }

};

#endif //POWERSUPPLY_ANALOGBUTTONS_H
