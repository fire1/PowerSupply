//
// Created by Angel Zaprianov on 2019-07-31.
//

#ifndef POWERSUPPLY_INPUTINTERFACE_H
#define POWERSUPPLY_INPUTINTERFACE_H

#include <Arduino.h>
#include "../PowerSupply.h"
#include "PowerController.h"
#include <EnableInterrupt.h>

#ifndef EnableInterrupt_h

#include "../../libraries/EnableInterrupt/EnableInterrupt.h"

#endif

#include <RotaryEncoder.h>

#ifndef RotaryEncoder_h

#include "../../libraries/RotaryEncoder/RotaryEncoder.h"

#endif

#include <AnalogButtons.h>

#ifndef ANALOGBUTTONS_H_

#include "../../libraries/Analog_Buttons/AnalogButtons.h"

#endif

#include "ButtonDrive.h"
#include "PresetMemory.h"


#ifndef FRAMES_PREVIEW
#define FRAMES_PREVIEW 4
#endif

void static encoderInterrupt();

class InputInterface {

    uint8_t cursor = 0;
    uint8_t frames = FRAMES_PREVIEW; // frames
    unsigned long timeout;
    PowerController *pc;
    AnalogButtons *ab;
    RotaryEncoder *enc;
    PresetMemory *pm;
    Preset preset;
private:


    void terminal() {
        if (Serial.available()) {
            String where = Serial.readStringUntil('=');

            if (where == F("v")) {
                analogWrite(pinVolPwm, Serial.readStringUntil('\n').toInt());
                Serial.println();
                Serial.print(F("VOLTAGE: "));
                Serial.print(pc->getSetVolt());
                Serial.println();
            }

            if (where == F("a")) {
                analogWrite(pinAmpPwm, Serial.readStringUntil('\n').toInt());
                Serial.println();
                Serial.print(F("CURRENT: "));
                Serial.print(pc->getSetAmps());
                Serial.println();
            }

            if (where == F("volt")) {
                pc->setVoltage(Serial.readStringUntil('\n').toFloat());
                Serial.println();
                Serial.print(F("VOLTAGE: "));
                Serial.print(pc->getSetVolt());
                Serial.println();
            }

            if (where == F("amps")) {
                pc->setAmperage(Serial.readStringUntil('\n').toFloat());
                Serial.println();
                Serial.print(F("CURRENT: "));
                Serial.print(pc->getSetAmps());
                Serial.println();
            }


            if (where == F("fan")) {
                int pwm = Serial.readStringUntil('\n').toInt();
                analogWrite(pinFans, pwm);
                Serial.println();
                Serial.print(F("FAn: "));
                Serial.print(pwm);
                Serial.println();
            }
        }
    }


    void ping() {
        timeout = millis() + editTimeout;
    }


    float changeValue(float value, int8_t direction, double rate = 1) {
        return (direction > 0) ? value + rate : value - rate;
    }

    void input() {

        int direction = enc->getDirection();
        if (direction != 0) {
            //
            // Editing SET
            if (!isPwm) {
                if (pc->menu.editAmps) {
                    double value = !pc->menu.editHalf ?: 0.050;
                    pc->setAmperage(changeValue(pc->getSetAmps(), direction, value));
                    tick();
                }

                if (pc->menu.editVolt) {
                    float value = !pc->menu.editHalf ?: 0.10;
                    pc->setVoltage(changeValue(pc->getSetVolt(), direction, value));
                    tick();
                }
                //
                // Editing PWM
            } else {

                uint8_t pwm;
                if (pc->menu.editVolt) {
                    uint8_t pwmV = pc->getPwmVolt();
                    pwm = (direction > 0) ? pwmV + 1 : pwmV - 1;
                    pc->setPwmVolt(pwm);
                    tick();
                }

                if (pc->menu.editAmps) {
                    uint8_t pwmA = pc->getPwmVolt();
                    pwm = (direction > 0) ? pwmA + 1 : pwmA - 1;
                    pc->setPwmAmps(pwm);
                    tick();
                }
            }
            ping();
        }

    }


    void buttons() {
        switch (currentButton) {
            case 1:
                cursor++;
                if (!isPwm) {
                    if (cursor > 4)
                        cursor = 1;
                } else {
                    if (cursor > 2)
                        cursor = 1;
                }

                tick();
                ping();
                break;
            case 11:
                if (isPwm) {
                    pc->isBat = true;
                    isPwm = false;
                    cursor = 0;
                } else {
                    cursor++;
                    isPwm = !isPwm;
                    pc->isBat = false;
                }
                alarm();
                break;

            case 2:
                if (edit) {
                    pc->mode.dynamic = (bool) !pc->mode.dynamic;
                } else {
                    pc->mode.powered = (bool) !pc->mode.powered;
                }
                tick();
                break;

            case 22:
                pc->mode.protect = (bool) !pc->mode.protect;
                alarm();
                break;

//// Load presets
            case 3:
                pm->get(1, preset);
                setPreset(preset);
                tick();
                break;
            case 4:
                pm->get(2, preset);
                setPreset(preset);
                tick();
                break;
            case 5:
                pm->get(3, preset);
                setPreset(preset);
                tick();
                break;
            case 6:
                pm->get(4, preset);
                setPreset(preset);
                tick();
                break;

//// Save presets
            case 33:
                pm->set(1, this->getPreset());
                alarm();
                break;
            case 44:
                pm->set(2, this->getPreset());
                alarm();
                break;
            case 55:
                pm->set(3, this->getPreset());
                alarm();
                break;
            case 66:
                pm->set(4, this->getPreset());
                alarm();
                break;
            default:
                return;
        }
        lastButton = currentButton;
        currentButton = 0;

    }

    Preset getPreset() {
        preset.amps = pc->getPwmAmps();
        preset.volt = pc->getPwmVolt();
        preset.mode = pc->getMode();
        return preset;
    }

    void setPreset(Preset preset) {
        pc->setPwmVolt(preset.volt);
        pc->setPwmAmps(preset.amps);
        pc->mode = preset.mode;

    }

    void noEdit() {
        cursor = 0;
        edit = false;
    }

public:
    boolean isPwm = false;
    boolean edit = false;
    uint8_t lastButton = 0;


    InputInterface(PowerController &cn, RotaryEncoder &ec, AnalogButtons &bt, PresetMemory &pm)
            : pc(&cn), enc(&ec), ab(&bt), pm(&pm) {}

    void begin() {


        enableInterrupt(pinEncoderA, encoderInterrupt, CHANGE);
        enableInterrupt(pinEncoderB, encoderInterrupt, CHANGE);

        pinMode(pinEncoderA, INPUT_PULLUP);
        pinMode(pinEncoderB, INPUT_PULLUP);
        pinMode(pinAnalogBt, INPUT_PULLUP);

        ab->add(btnEncoder);
        ab->add(btnBlinker);
        ab->add(btnMemSetA);
        ab->add(btnMemSetB);
        ab->add(btnMemSetC);
        ab->add(btnMemSetD);
    }


    void listen() {
        this->terminal();
        ab->check();
        this->buttons();

        if (cursor > 0) {
            this->input();
            if (timeout <= millis()) {
                noEdit();
            }
        }
    }


    uint8_t getCursor() {
        return cursor;
    }

    uint8_t getSaved() {
        uint8_t saved = pm->getLastSaved();
        return saved;
    }

    uint8_t getLoaded() {
        uint8_t loaded = pm->getLastLoaded();
        return loaded;
    }

};

#endif //POWERSUPPLY_INPUTINTERFACE_H
