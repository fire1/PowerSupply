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

    Preset saved;
    uint8_t lastIndexSave = 0;
    uint8_t lastIndexLoad = 0;

public:


    void set(uint8_t setIndex, Preset save) {

        if (saved.volt != save.volt || saved.amps != save.amps || lastIndexSave != setIndex) {
            EEPROM.put(setIndex * sizeof(Preset), save);
            saved = save;
            lastIndexSave = setIndex;
        }
        delay(5);
    }


/*    void set(uint8_t setIndex, float voltage, float amperage, uint8_t mode) {
        Preset save = {
                voltage, amperage, mode
        };

        if (saved.volt != save.volt || saved.amps != save.amps || lastIndexSave != setIndex) {
            EEPROM.put(setIndex * sizeof(Preset), save);
            saved = save;
            lastIndexSave = setIndex;
        }
    }*/

    void get(uint8_t setIndex, Preset &output) {
        lastIndexLoad = setIndex;
        EEPROM.get(setIndex * sizeof(Preset), output);
    }


    uint8_t getLastLoaded() {
        return lastIndexLoad;
    }


    void clearLastLoaded() {
        lastIndexLoad = 0;
    }

    uint8_t getLastSaved() {
        return lastIndexSave;
    }

    void clearLastSaved() {
        lastIndexSave = 0;
    }


};

#endif //POWERSUPPLY_PRESETMEMORY_H
