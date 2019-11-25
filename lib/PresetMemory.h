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
    float amps;
    PowerMode mode;
};

class PresetMemory {

    Preset lastSave;
    uint8_t lastIndexSave = 0;

public:


    void set(uint8_t setIndex, Preset save) {

        if (lastSave.volt != save.volt || lastSave.amps != save.amps || lastIndexSave != setIndex) {
            EEPROM.put(setIndex * sizeof(Preset), save);
            lastSave = save;
            lastIndexSave = setIndex;
        }
        delay(5);
    }


/*    void set(uint8_t setIndex, float voltage, float amperage, uint8_t mode) {
        Preset save = {
                voltage, amperage, mode
        };

        if (lastSave.volt != save.volt || lastSave.amps != save.amps || lastIndexSave != setIndex) {
            EEPROM.put(setIndex * sizeof(Preset), save);
            lastSave = save;
            lastIndexSave = setIndex;
        }
    }*/

    void get(uint8_t setIndex, Preset &output) {
        EEPROM.get(setIndex * sizeof(Preset), output);
    }


};

#endif //POWERSUPPLY_PRESETMEMORY_H
