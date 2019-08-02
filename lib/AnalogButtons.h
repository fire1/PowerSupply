//
// Created by Admin on 8/2/2019.
//

#ifndef POWERSUPPLY_ANALOGBUTTONS_H
#define POWERSUPPLY_ANALOGBUTTONS_H

#include <Arduino.h>
#include "../PowerSupply.h"

typedef  uint8_t btn;

#ifndef ButtonCheckInterval
#define ButtonCheckInterval 1000
#endif

#ifndef ButtonDebounceInterval
#define ButtonDebounceInterval 350
#endif

class AnalogButtons {
    btn now = 0;
    btn last = 0;
    int read, compare = 0;
    unsigned long lastCheck = 0;

private:

    void detectEncoder() {
        if (read > 550 && read < 650) {
            now = btnEncoder;
        }

        if (last == now) {
            now = btnEntries;
        }
    }

public:

    const static btn btnNone = 0;
    const static btn btnEncoder = 1;
    const static btn btnEntries = 2;

    void begin() {
        pinMode(pinAnalogBt, INPUT_PULLUP);
    }

    void listener() {

        if (currentMillis - lastCheck >= ButtonCheckInterval) {
            lastCheck = currentMillis;
            compare = analogRead(pinAnalogBt);
            read = 0;
        }

        if (compare > 0 && currentMillis - lastCheck >= ButtonDebounceInterval) {
            read = analogRead(pinAnalogBt);
            if (compare / 10 == read / 10) {
                detectEncoder();
            }
            compare = 0;

        }


    }

    btn getButton() {
        last = now;
        now = btnNone;
        return last;
    }

};

#endif //POWERSUPPLY_ANALOGBUTTONS_H
