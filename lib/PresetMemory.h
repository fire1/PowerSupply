//
// Created by Angel Zaprianov on 2019-08-15.
//

#ifndef POWERSUPPLY_PRESETMEMORY_H
#define POWERSUPPLY_PRESETMEMORY_H

#include <EEPROM.h>

#include "PowerController.h"

#define MEM_STEP 3


struct Preset {
    float volt;
    float amp;
    uint8_t mode;
};

class PresetMemory {



public:


    void set(uint8_t setIndex, Preset save) {
        EEPROM.put(setIndex + sizeof(Preset), save);
        delay(5);
    }


    void set(uint8_t setIndex, float voltage, float amperage, uint8_t mode) {
        Preset save = {
                voltage, amperage, mode
        };
        EEPROM.put(setIndex * sizeof(Preset), save);
    }

    void get(uint8_t setIndex, Preset &output) {
        EEPROM.get(setIndex * sizeof(Preset), output);
    }


};

#endif //POWERSUPPLY_PRESETMEMORY_H
