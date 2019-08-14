//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWER_SUPPLY_FUNCTIONS_H
#define POWER_SUPPLY_FUNCTIONS_H

#include <Arduino.h>
#include "../PowerSupply.h"


ResponsiveAnalogRead rawVolt(pinVolt, true);
ResponsiveAnalogRead rawAmps(pinAmps, true);

//  pwm     / volt
//
//  100         26
//  62          25
//  61          21
//  60          18
//  59          17.4
//  57          16.1
//  56          14.6
//  54          13.1
//  53          12.3
//  52          11
//  50          8.6
//  49          7
//  47          5.6
//  46          4.8
//  45          4.1
//  44          3.2

#ifndef maxPwmValue
#define maxPwmValue  255;
#endif

#ifndef minPwmValue
#define minPwmValue 1
#endif

#ifndef pwmSwtCorrection
#define pwmSwtCorrection 0.30
#endif


class PowerController {

    boolean activeParse = true;
    boolean isModeChange = true;
    boolean isActiveLinear = false;
    volatile uint8_t index;
    volatile uint8_t offset;
    uint8_t powerMode = PowerController::MODE_SWT_PW; // liner, power switching, limited switching
    uint8_t pwmValue = 1;
    uint8_t maxPwmControl = maxPwmValue;
    uint8_t minPwmControl = minPwmValue;
    uint8_t lastTrVoltage = 0;
    float targetVolt = 3;
    float liveVolts = 0;
    float readVolts = 0;
    float correctionValue = 0;
    uint32_t ampSmoothIndex;
    int dumpVolts, dumpAmps;
    int readAmps = 0;
    double liveAmps = 0;
    double targetAmps = 0.200;
    double ampSmooth = 0;
    double captureAmp;


    void setupPwm() {
        pinMode(pinPWM, OUTPUT);
        //---------------------------------------------- Set PWM frequency for D5 & D6 -------------------------------

        pinMode(3, OUTPUT); // Output pin for OCR2B
        pinMode(5, OUTPUT); // Output pin for OCR0B

//        TCCR0B = TCCR0B & 0b11111000 | 0x05;
        TCCR0B = TCCR0B & B11111000 | B00000001;    // set timer 0 divisor to     1 for PWM frequency of 62500.00 Hz
//    TCCR0B = TCCR0B & B11111000 | B00000010;    // set timer 0 divisor to     8 for PWM frequency of  7812.50 Hz
//    TCCR0B = TCCR0B & B11111000 | B00000011;    // set timer 0 divisor to    64 for PWM frequency of   976.56 Hz
//TCCR0B = TCCR0B & B11111000 | B00000100;    // set timer 0 divisor to   256 for PWM frequency of   244.14 Hz
//TCCR0B = TCCR0B & B11111000 | B00000101;    // set timer 0 divisor to  1024 for PWM frequency of    61.04 Hz

        //---------------------------------------------- Set PWM frequency for D3 & D11 ------------------------------

        TCCR2B = TCCR2B & B11111000 | B00000001;    // set timer 2 divisor to     1 for PWM frequency of 31372.55 Hz
//TCCR2B = TCCR2B & B11111000 | B00000010;    // set timer 2 divisor to     8 for PWM frequency of  3921.16 Hz
//TCCR2B = TCCR2B & B11111000 | B00000011;    // set timer 2 divisor to    32 for PWM frequency of   980.39 Hz
//    TCCR2B = TCCR2B & B11111000 | B00000100;    // set timer 2 divisor to    64 for PWM frequency of   490.20 Hz (The DEFAULT)
//TCCR2B = TCCR2B & B11111000 | B00000101;    // set timer 2 divisor to   128 for PWM frequency of   245.10 Hz
//TCCR2B = TCCR2B & B11111000 | B00000110;    // set timer 2 divisor to   256 for PWM frequency of   122.55 Hz
//TCCR2B = TCCR2B & B11111000 | B00000111;    // set timer 2 divisor to  1024 for PWM frequency of    30.64 Hz


        //---------------------------------------------- Set PWM frequency for D9 & D10 ------------------------------
        TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
//        TCCR1B = TCCR1B & B11111000 | B00000101;    // set timer 1 divisor to  1024 for PWM frequency of    30.64 Hz
//        TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
    }


    float toVoltage(int raw) {
        return map(raw, 140, 940, 411, 2700) * 0.01;
    }

    void readValues() {
        for (index = 0; index < 3; ++index) {
            rawVolt.update();
            rawAmps.update();
        }
    }

    void sensVolts() {
        readValues();
        dumpVolts = readVolts = rawVolt.getRawValue();
        liveVolts = this->toVoltage(readVolts);

    }


    void sensAmps() {
        dumpAmps = readAmps = rawAmps.getValue();
        float deflectVolt = map((int) liveVolts, 0, 30, 935, 1196) * 0.1; // deflection curve by voltage
        captureAmp = readAmps * deflectVolt * 0.1;


        if (readAmps < 30) {
            liveAmps = map(readAmps, 19, 40, 120, 250);
        } else if (readAmps < 61) {
            liveAmps = map(readAmps, 20, 45, 205, 630);
        } else if (readAmps > 60) {
            liveAmps = map(readAmps, 56, 550, 670, 5700);
        }
//        liveAmps = liveAmps < 0 ? 0 : liveAmps * 0.001;
        liveAmps = captureAmp * 0.001;
        ampSmooth += liveAmps;
        ampSmoothIndex++;
    }

    uint8_t lastPwm = 0;


    void resolveMaxPwmValue() {
        if (isModeChange) {
//            maxPwmControl = voltTable[uint8_t(targetVolt)];

        } else if (!isModeChange) {
//            maxPwmControl = maxPwmValue;
        }
        lastTrVoltage = uint8_t(targetVolt);
    }

    void resolvePowerMode() {
        if (isModeChange) {
            powerMode = PowerController::MODE_SWT_LM;
        }
        if (!isModeChange) {
            powerMode = PowerController::MODE_SWT_PW;
        }
        if (isActiveLinear) {
            powerMode = PowerController::MODE_LIN_PW;
        }
    }

/**
 *
 * @param pwm
 */
    void setPwm(uint8_t pwm) {
        lastPwm = pwm;
        analogWrite(pinPWM, pwm);
    }


    void parsePwmSwitching() {
        if (!activeParse) return;

        if (liveAmps < targetAmps) {
            if (targetVolt < liveVolts) {
                //
                // Pump up voltage
                pwmValue = pwmValue - 1;
                pwmValue = constrain(pwmValue, minPwmControl, maxPwmControl);
            } else if (targetVolt + correctionValue > liveVolts) {
                //
                // Lower the voltage
                pwmValue = pwmValue + 1;
                pwmValue = constrain(pwmValue, minPwmControl, maxPwmControl);
            }
        }

        //
        // Lower voltage at high amps limit
        if (liveAmps > targetAmps) {
            pwmValue = pwmValue - 1;
            pwmValue = constrain(pwmValue, minPwmControl, maxPwmControl);
        }
    }

public:

    static const uint8_t MODE_SWT_PW = 0;
    static const uint8_t MODE_SWT_LM = 1;
    static const uint8_t MODE_LIN_PW = 2;
    static const uint8_t MODE_LIN_LM = 3;

/**
 *
 * @param pin
 * @param divisor
 */
    void setPwmFrequency(int pin, int divisor) {
        byte mode;
        if (pin == 5 || pin == 6 || pin == 9 || pin == 10) {
            switch (divisor) {
                case 1:
                    mode = 0x01;
                    break;
                case 8:
                    mode = 0x02;
                    break;
                case 64:
                    mode = 0x03;
                    break;
                case 256:
                    mode = 0x04;
                    break;
                case 1024:
                    mode = 0x05;
                    break;
                default:
                    return;
            }
            if (pin == 5 || pin == 6) {
                TCCR0B = TCCR0B & 0b11111000 | mode;
            } else {
                TCCR1B = TCCR1B & 0b11111000 | mode;
            }
        } else if (pin == 3 || pin == 11) {
            switch (divisor) {
                case 1:
                    mode = 0x01;
                    break;
                case 8:
                    mode = 0x02;
                    break;
                case 32:
                    mode = 0x03;
                    break;
                case 64:
                    mode = 0x04;
                    break;
                case 128:
                    mode = 0x05;
                    break;
                case 256:
                    mode = 0x06;
                    break;
                case 1024:
                    mode = 0x7;
                    break;
                default:
                    return;
            }
            TCCR2B = TCCR2B & 0b11111000 | mode;
        }
    }

    void begin() {
        setupPwm();
    }

    void manage() {
        sensVolts();
        sensAmps();
        resolvePowerMode();
        if ((liveVolts > targetVolt + 1 || liveVolts < targetVolt - 1)) {
            offset = 0;
            while (liveVolts > targetVolt + 1 && liveVolts < targetVolt - 1 && offset < 228) {

                parsePwmSwitching();
                sensVolts();
                setPwm(pwmValue);
                digitalWrite(pinLed, HIGH);
                delayMicroseconds(1);
                offset++;
            }
        }

        digitalWrite(pinLed, LOW);
        parsePwmSwitching();
        pinMode(pinVolt, INPUT);
        pinMode(pinAmps, INPUT);
        pinMode(pinLed, OUTPUT);
        analogWrite(pinPWM, 0);
        setPwm(pwmValue);
    }

    void setVoltage(float value) {
        if (value >= 0 && value <= 26)
            targetVolt = value;
    }

    void setAmperage(float value) {
        if (value >= 0 && value <= 10)
            targetAmps = value;
    }

/**
 *
 * @param value
 */
    void setPWM(uint8_t value) {
        pwmValue = value;
    }

    float getLiveVolt() {
        return liveVolts;
    }

    float getLiveAmps() {
        return liveAmps;
    }

    void useParse(boolean active) {
        activeParse = active;
    }


    void togglePwmMode() {
        this->isModeChange = !this->isModeChange;
        correctionValue = (isModeChange != 0) ? pwmSwtCorrection : correctionValue;
    }


    int getDumpVolt() {
        return dumpVolts;
    }

    int getDumpAmps() {
        return dumpAmps;
    }

    float getTargetVolt() {
        return targetVolt;
    }

    float getTargetAmps() {
        return targetAmps;
    }

    uint8_t getPwmValue() {
        return pwmValue;
    }


    double lcdVolt() {
        return toVoltage(rawVolt.getValue());
    }

    double lcdAmps() {
        double result = ampSmooth / ampSmoothIndex;
        ampSmooth = result;
        ampSmoothIndex = 1;
        return result;
    }

    uint8_t getPowerMode() {
        return powerMode;
    }

    double testAmperage() {
        return captureAmp;
    }
};
/*
#define avrGap 10

uint8_t capMin, capMax, capAvr, capIndex = 0;
uint8_t capContainer[100];

void setToAvr(uint8_t val) {
    capContainer[capIndex] = val;
    capIndex++;

    if (capIndex > 100) {
        capIndex = 0;
    }
}


// Print array elements greater than average
void parseAvg(int arr[], int n) {
    // Find average
    double avg = 0;
    for (int i = 0; i < n; i++)
        avg += arr[i];
    avg = avg / n;
    capAvr = int(avg);

    // Print elements greater than average
    for (int i = 0; i < n; i++)
        if (arr[i] >= avg - avrGap && arr[i] <= avg + avrGap) {
            capMin = capMin > arr[i] ? arr[i] : capMin;
            capMax = capMax < arr[i] ? arr[i] : capMax;
        }
}*/

#endif //POWERSUPPLY_FUNCTIONS_H






