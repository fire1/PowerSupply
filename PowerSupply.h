//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWER_SUPPLY_H
#define POWER_SUPPLY_H

#include <Arduino.h>
#include <ResponsiveAnalogRead.h>

#include <LiquidCrystal.h>



#include <LiquidCrystal.h>

#ifndef LiquidCrystal_4bit_h

#include "../libraries/NewLiquidCrystal_lib/LiquidCrystal.h"

#endif

#ifndef RESPONSIVE_ANALOG_READ_H

//#include "../libraries/ResponsiveAnalogRead/src/ResponsiveAnalogRead.h"

#endif

/*
 * PWM 9,10
 * tone: 11 (or 3)
 * LCD: 2,4,5,6,7,8
 * LED: 13
 * Buttons: A0
 * Encoder: A1,A2
 * Thermistor: A3
 * Fan: 3 (or 11)
 */
LiquidCrystal lcd(2, 4, 5, 6, 7, 8);
//Inputs/outputs
boolean fastScreen = false;
const uint8_t pinVolInp = A1;
const uint8_t pinAmpInp = A0;
const uint8_t pinVolPwm = 9;
const uint8_t pinAmpPwm = 10;
const uint8_t pinAnalogBt = A0;
const uint8_t pinEncoderB = A1;
const uint8_t pinEncoderA = A2; // only input!
const uint8_t pinLed = 13;
const uint8_t pinThermistorSwt = A6;
const uint8_t pinThermistorLin = A2;
const uint8_t pinFans = 3; //alert pin INA
const uint8_t pinTone = 11; // timer pwm

const uint16_t screenNormalRefresh = 650;
const uint16_t screenEditorRefresh = 160;
const uint16_t editTimeout = 10000;
const uint16_t holdTimeout = 400;
const float thresholdVoltage = 0.05;
unsigned long futureMillis = 0;
volatile unsigned long currentLoops = 0;
uint8_t heatSwt, heatLin;


char printValues[6];

byte charLinear[] = {
        B00000,
        B11111,
        B00000,
        B10101,
        B00000,
        B00000,
        B00000,
        B00000
};
byte charLimits[] = {
        B00000,
        B11111,
        B00000,
        B01110,
        B01010,
        B01010,
        B11011,
        B00000
};
byte charSwitch[] = {
        B00000,
        B01110,
        B01010,
        B01010,
        B01010,
        B01010,
        B11011,
        B00000
};
byte charPlugin[] = {
        B01010,
        B01010,
        B11111,
        B10001,
        B01110,
        B00100,
        B00100,
        B00000
};

unsigned long amplitude10, amplitude100, amplitude350;

boolean is10() {
    if (amplitude10 < currentLoops) {
        amplitude10 = currentLoops + 10;
        return true;
    }
}

boolean is80() {
    if (amplitude100 < currentLoops) {
        amplitude100 = currentLoops + 80;
        return true;
    }
}

boolean is250() {
    if (amplitude100 < currentLoops) {
        amplitude350 = currentLoops + 250;
        return true;
    }
}


void fansControl() {
    int rawTempSwt = analogRead(pinThermistorSwt);
    int rawTempLin = analogRead(pinThermistorLin);
//    heatSwt = (uint8_t) map(rawTempSwt, 345, 460, 120, 70);
//    heatLin = (uint8_t) map(rawTempLin, 345, 460, 120, 70);
    heatSwt = (uint8_t) map(rawTempSwt, 345, 656, 120, 18);
    heatLin = (uint8_t) map(rawTempLin, 345, 656, 120, 18);

    uint8_t heat = (heatLin > heatSwt) ? heatLin : heatSwt;
    if (heat > 60) {
        uint8_t fpw = map(heat, 35, 100, 0, 254);
        fpw = (fpw > 254) ? 254 : fpw;
        fpw = (fpw < 2) ? 1 : fpw;
        analogWrite(pinFans, fpw);
    } else if (heat > 35 && heat < 60) {
        analogWrite(pinFans, map(heat, 35, 65, 1, 3));
    } else if (heatSwt < 25 && heatLin < 25) {
        analogWrite(pinFans, 0);
    }
}

//Other Variables

//
// ADC lower the resolution increase speed
//
// ADC clock of 1 MHz and a sample rate of ~77KHz
// without much loss of resolution.


// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void fastADC() {
// set prescale to 16
    sbi(ADCSRA, ADPS2);
    cbi(ADCSRA, ADPS1);
    cbi(ADCSRA, ADPS0);
}

boolean blinker = 0;

void blink() {
    blinker = !blinker;
    digitalWrite(pinLed, blinker);
}

unsigned long startAlarmed = 0;

void alarm() {
    if (startAlarmed == 0) {
        analogWrite(pinTone, 200);
        startAlarmed = currentLoops;
    }
}

void alert() {
    alarm();
}

void noAlarm() {
    if (currentLoops > startAlarmed + 250 ) {
        analogWrite(pinTone, 0);
        startAlarmed = 0;
    }
}

#endif //POWERSUPPLY_POWERSUPPLY_H
