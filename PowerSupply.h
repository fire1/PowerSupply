//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWER_SUPPLY_H
#define POWER_SUPPLY_H
/*
 * Deprecated use of ResponsiveAnalogRead
 * */



#include <Arduino.h>


#include <LiquidCrystal.h>

#include <LiquidCrystal.h>

#ifndef LiquidCrystal_4bit_h

#include "../libraries/NewLiquidCrystal_lib/LiquidCrystal.h"

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
// free pins A7
boolean fastScreen = false;
const uint8_t pinLed = 13;
const uint8_t pinTone = 3;
const uint8_t pinFans = 11; // timer pwm
const uint8_t pinAmpPwm = 9;
const uint8_t pinVolPwm = 10;
const uint8_t pinInaAlert = 12; // alert pin ina
const uint8_t pinAmpLimit = A0;
const uint8_t pinAnalogBt = A1;
const uint8_t pinEncoderB = A2;
const uint8_t pinEncoderA = A3;
const uint8_t pinHeatTemp = A6; // 4.7к pull up resistor

const uint16_t screenNormalRefresh = 450;
const uint16_t screenEditorRefresh = 350;
const uint16_t editTimeout = 6000;
const uint16_t holdTimeout = 400;
unsigned long futureMillis = 0;
volatile unsigned long currentTime = 0;
volatile uint8_t index;
volatile uint8_t ampLimiter = 0;
int16_t temperature;


char printValues[6];




unsigned long amplitude20, amplitude100, amplitude350;

boolean is20() {
    if (amplitude20 < currentTime) {
        amplitude20 = currentTime + 10;
        return true;
    }
}

boolean is80() {
    if (amplitude100 < currentTime) {
        amplitude100 = currentTime + 80;
        return true;
    }
}

boolean is250() {
    if (amplitude100 < currentTime) {
        amplitude350 = currentTime + 250;
        return true;
    }
};

void fansControl() {
    int rawTemp = analogRead(pinHeatTemp);
    temperature = (uint8_t) map(rawTemp, 325, 585, 120, 14);
    if (temperature > 60) {
        uint8_t fpw = (uint8_t) map(temperature, 35, 95, 25, 254);
        fpw = (fpw > 254) ? 254 : fpw;
        fpw = (fpw < 2) ? 1 : fpw;
        analogWrite(pinFans, fpw);

    } else if (temperature < 35) {
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
int16_t soundLength = 0;

void alarm() {
    if (startAlarmed == 0) {
        analogWrite(pinTone, 200);
        startAlarmed = millis();
        soundLength = 450;
    }
}

void tick() {
    if (startAlarmed == 0) {
        analogWrite(pinTone, 20);
        startAlarmed = millis();
        soundLength = 50;
    }
}

void noAlarm() {
    if (millis() > startAlarmed + soundLength) {
        analogWrite(pinTone, 0);
        startAlarmed = 0;
    }
}

#endif //POWERSUPPLY_POWERSUPPLY_H
