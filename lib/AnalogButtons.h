//
// Created by Admin on 8/2/2019.
//

#ifndef POWERSUPPLY_ANALOGBUTTONS_H
#define POWERSUPPLY_ANALOGBUTTONS_H

#include <Arduino.h>
#include "../PowerSupply.h"

typedef uint8_t btn;

#ifndef ButtonCheckInterval
#define ButtonCheckInterval 500
#endif

#ifndef ButtonCheckIndexInt
#define ButtonCheckIndexInt 4
#endif

#ifndef ButtonDebounceInterval
#define ButtonDebounceInterval 5000
#endif

class AnalogButtons {
    btn now = 0;
    btn last = 0;
    uint8_t index = 0;
    int read, compare = 0;
    unsigned long lastCheck = 0;

private:

    void detectEncoder() {
        if (compare > 600 && compare < 700 && last != now) {
            now = btnEncoder;
            Serial.print(F(" Encoder Click "));
        }

        if (last == now && read > 600 && read < 700) {
            now = btnEntries;
            Serial.print(F(" Encoder 2 Click "));
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

        if (currentMillis - lastCheck >= ButtonCheckInterval && index <= ButtonCheckIndexInt) {
            lastCheck = currentMillis;
            compare += analogRead(pinAnalogBt);
            read = 0;
            index++;
        }

        if (index >= ButtonCheckIndexInt && compare > 0) {
            compare = compare / index;
            index = 0;

            if (compare > 0 && compare < 950) {
                lastCheck = currentMillis + ButtonCheckInterval;
                detectEncoder();
                compare = 0;

            }
        }
    }

    btn getButton() {
        last = now;
        now = btnNone;
        return last;
    }

};

#endif //POWERSUPPLY_ANALOGBUTTONS_H
