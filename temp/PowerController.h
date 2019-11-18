//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWER_SUPPLY_FUNCTIONS_H
#define POWER_SUPPLY_FUNCTIONS_H

#include <Arduino.h>
#include "../PowerSupply.h"


ResponsiveAnalogRead rawVolt(pinVolInp, true);
ResponsiveAnalogRead rawAmps(pinAmpInp, true);

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


#define avrSimples 100
#define avrGap 50

#ifndef maxPwmValue
#define maxPwmValue  200 //200
#endif

#ifndef minPwmValue
#define minPwmValue 3
#endif

#ifndef pwmSwtCorrection
#define pwmSwtCorrection 0.30
#endif


class PowerController {

    boolean activeParse = true;
    boolean isModeChange = true;
    boolean isActiveLinear = false;
    boolean isPowered = true;
    volatile uint8_t index;
    volatile uint8_t offset;
    uint8_t lastPwm = 0;
    uint8_t powerMode = PowerController::MODE_SWT_PW; // liner, power switching, limited switching
    uint8_t pwmValue = 0;
    uint8_t maxPwmControl = maxPwmValue;
    uint8_t minPwmControl = minPwmValue;
    uint8_t overloading = 0;
    uint8_t capMin = 255, capMax, capAvr, capIndex = 0;
    uint8_t capContainer[100];
    boolean avrCalibration = false;
    float setVolt = 3;
    float liveVolts = 0;
    float readVolts = 0;
    float correctionValue = 0;
    uint32_t ampSmoothIndex;
    int dumpVolts, dumpAmps;
    int readAmps = 0;
    double liveAmps = 0;
    double setAmps = 0.200;
    double ampSmooth = 0;
    double captureAmp;


    void setupPwm() {
        //---------------------------------------------- Set PWM frequency for D5 & D6 -------------------------------
        pinMode(pinVolPwm, OUTPUT); // Output pin for OCR0B

//        Timer 2 fast PWM
//        TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
//        TCCR2B = _BV(WGM22) | _BV(CS20);
//        OCR2A = maxPwmValue;// 63
//        OCR2B = 0;


        //---------------------------------------------- Set PWM frequency for D9 & D10 ------------------------------
        TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz

    }


    static float toVoltage(int raw) {
        return float(map(raw, 140, 940, 411, 2700) * 0.01);
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
        float crV = map(liveVolts, 1, 25, -1, 1) * 0.1;
        liveVolts -= crV;
    }


    void sensAmps() {
        dumpAmps = readAmps = rawAmps.getValue();
        float deflectVolt = map((int) liveVolts, 0, 30, 935, 1196) * 0.1; // deflection curve by voltage
        captureAmp = readAmps * deflectVolt * 0.1;


        if (readAmps < 30) {
            liveAmps = map(readAmps, 19, 40, 140, 250);
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

        if (pwm > maxPwmValue) {
            pwm = maxPwmValue;
        }
        setToAvr(pwm);
        OCR2B = pwm; // move it here constrain(pwm, minPwmControl, maxPwmControl)
        lastPwm = pwm;
        analogWrite(pinVolPwm, pwm);
    }


    void parsePwmSwitching() {
        if (!activeParse) return;
        //
        // High amps limit
        if (liveAmps > setAmps) {
            pwmValue = pwmValue - 2;
            pwmValue = constrain(pwmValue, minPwmControl, maxPwmControl);
            overloading++;
        }


        if (overloading > 60) {
            pwmValue = 0;
            activeParse = false;
            alarm();
        }


        if (this->isAvrCalibrated()) {
            parseAvg(capContainer, avrSimples);
            pwmValue = capAvr;
            blink();
            return;
        }

        //
        // Low amps
        if (liveAmps <= setAmps) {
            overloading = 0;
            int gap = setVolt * 100 - liveVolts * 100;
            //
            // Voltage is in range
            if (abs(gap) < 10) return;

            if (setVolt < liveVolts && gap > 100) {
                maxPwmControl = lastPwm;
                pwmValue = pwmValue - 3;
                pwmValue = constrain(pwmValue, minPwmControl, maxPwmControl);
            } else if (setVolt < liveVolts) {
                //
                // Pump up voltage
                pwmValue = pwmValue - 1;
                pwmValue = constrain(pwmValue, minPwmControl, maxPwmControl);
            } else if (setVolt > liveVolts) {
                //
                // Lower the voltage
                pwmValue = pwmValue + 1;
                pwmValue = constrain(pwmValue, minPwmControl, maxPwmControl);
            }
        }


    }


    boolean isAvrCalibrated() {
        if (avrCalibration) {
            avrCalibration = false;
            return true;
        }
        return false;
    }

    void setToAvr(uint8_t val) {
        capContainer[capIndex] = val;
        capIndex++;

        if (capIndex > avrSimples) {
            capIndex = 0;
            avrCalibration = true;
        }
    }

    void parseAvg(uint8_t *arr, int n) {
        // Find average
        double avg = 0;
        for (index = 0; index < n; index++)
            avg += arr[index];
        avg = avg / n;
        capAvr = int(avg);

        // Print elements greater than average
        for (index = 0; index < n; index++)
            if (arr[index] >= avg - avrGap && arr[index] <= avg + avrGap) {
                capMin = capMin > arr[index] ? arr[index] : capMin;
                capMax = capMax < arr[index] ? arr[index] : capMax;
            }

        if (capMin - 5 > minPwmValue + 1)
            minPwmControl = capMin - 1;
        if (capMax + 5 < maxPwmValue)
            maxPwmControl = capMax;

    }


public:

    static const uint8_t MODE_SWT_PW = 0;
    static const uint8_t MODE_SWT_LM = 1;
    static const uint8_t MODE_LIN_PW = 2;
    static const uint8_t MODE_LIN_LM = 3;


    void begin() {
        setupPwm();
        pinMode(pinVolInp, INPUT);
        pinMode(pinAmpInp, INPUT);
        pinMode(pinLed, OUTPUT);
    }

    void manage() {
        if (!isPowered) {
            setPWM(0);
            return;
        }
        sensVolts();
        sensAmps();
        resolvePowerMode();
        parsePwmSwitching();
        setPwm(pwmValue);
    }

    void resetControl() {
        maxPwmControl = maxPwmValue;
        minPwmControl = minPwmValue;
    }

    void setVoltage(float value) {
        resetControl();
        if (value >= 0 && value < 26)
            setVolt = value;
    }

    void setAmperage(float value) {
        resetControl();
        if (value >= 0 && value <= 5)
            setAmps = value;
    }

/**
 *
 * @param value
 */
    void setPWM(uint8_t value) {
        pwmValue = value;
    }

    float lcdVolt() {
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

    float getSetVolt() {
        return setVolt;
    }

    float getSetAmps() {
        return setAmps;
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

    void power(boolean state) {
        if (state) {
            pwmValue = 15;
            isPowered = true;
        } else {
            isPowered = false;
        }
    }

};
/*
 * For testing by averaging value of pwm
 *
 *
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





