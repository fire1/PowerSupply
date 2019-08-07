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
#define ButtonDebounceInterval 15000
#endif

class AnalogButtons {
    btn now = 0;
    btn last = 0;
    uint8_t index = 0;
    int read;
    unsigned long lastCheck = 0, compare = 0;

private:

    void detectEncoder() {
        if (read > 600 && read < 700 && last != now) {
            now = btnEncoder;
            Serial.print(F(" Encoder Click "));
        }

/*        if (last == now && read > 600 && read < 700) {
            now = btnEntries;
            Serial.print(F(" Encoder 2 Click "));
        }*/
    }

    void detectBlinker() {
        if (read > 20 && read < 100 && last != now) {
            now = btnBlinker;
            Serial.print(F(" Blinker Click "));
        }
    }

    boolean isDebounce() {
        if (read > 0) {
            return currentMillis - lastCheck > ButtonDebounceInterval;
        }
        return true;
    }

public:

    const static btn btnNone = 0;
    const static btn btnEncoder = 1;
    const static btn btnEntries = 2;
    const static btn btnBlinker = 3;

    void begin() {
        pinMode(pinAnalogBt, INPUT_PULLUP);
    }

    void listener() {

        if (now == btnNone) {
            if (currentMillis - lastCheck >= ButtonCheckInterval && index <= ButtonCheckIndexInt && isDebounce()) {
                lastCheck = currentMillis;
                read = 0;
                compare += analogRead(pinAnalogBt);
                index++;

            }

            if (index >= ButtonCheckIndexInt && compare > 0) {
                compare += analogRead(pinAnalogBt);
                index++;
                read = (int)compare / index;
                index = 0;

                if (read > 0 && read < 980) {
                    lastCheck = currentMillis + ButtonCheckInterval;
                    detectEncoder();
                    detectBlinker();
                    compare = 0;
                }
            }
        }
    }

    int getReadings() {
        return read;
    }

    btn getButton() {
        last = now;
        now = btnNone;
        return last;
    }

};

#endif //POWERSUPPLY_ANALOGBUTTONS_H
