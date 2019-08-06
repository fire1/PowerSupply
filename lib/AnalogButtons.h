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
#define ButtonCheckIndexInt 24
#endif

#ifndef ButtonDebounceInterval
#define ButtonDebounceInterval 5000
#endif

class AnalogButtons {
    btn now = 0;
    btn last = 0;
    uint8_t index = 0;
    int read;
    unsigned long lastCheck = 0,compare = 0;

private:

    void detectEncoder() {
        if (read > 600 && read < 700 && last != now) {
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
            read = 0;
            compare += analogRead(pinAnalogBt);
            index++;

        }

        if (index >= ButtonCheckIndexInt && compare > 0) {
            compare += analogRead(pinAnalogBt);
            index++;
            compare = compare / index;
            index = 0;

            if (compare > 0 && compare < 980) {
                read = compare;
                lastCheck = currentMillis + ButtonCheckInterval;
                detectEncoder();
                compare = 0;

            }
        }
    }

    int getReadings(){
        return  read;
    }

    btn getButton() {
        last = now;
        now = btnNone;
        return last;
    }

};

#endif //POWERSUPPLY_ANALOGBUTTONS_H
