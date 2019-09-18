//
// Created by Angel Zaprianov on 2019-07-16.
//

#ifndef POWER_SUPPLY_H
#define POWER_SUPPLY_H

#include <Arduino.h>
#include <ResponsiveAnalogRead.h>

#ifndef RESPONSIVE_ANALOG_READ_H

#include "../libraries/ResponsiveAnalogRead/src/ResponsiveAnalogRead.h"

#endif

#include <LiquidCrystal.h>


//Inputs/outputs
boolean fastScreen = false;
const uint8_t pinVolt = A1;
const uint8_t pinAmps = A0;
const uint8_t pinSwhPwm = 5;
const uint8_t pinLinPwm = 6;
const uint8_t pinEncoderB = 2;
const uint8_t pinEncoderA = 3;
const uint8_t pinAnalogBt = A3;
const uint16_t screenNormalRefresh = 310;
const uint16_t screenEditorRefresh = 120;
const uint8_t pinLed = 13;
const uint8_t pinThermistorSwt = A6;
const uint8_t pinThermistorLin = A2;
const uint8_t pinFans = 10;
const uint8_t pinTone = 11;
const uint16_t editTimeout = 10000;
const uint16_t holdTimeout = 400;
const float thresholdBoost = 0.3;
unsigned long previousMillis = 0;
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
    heatSwt = (uint8_t) map(rawTempSwt, 345, 460, 120, 70);
    heatLin = (uint8_t) map(rawTempLin, 345, 460, 120, 70);

    uint8_t heat = (heatLin > heatSwt) ? heatLin : heatSwt;
    if (heat > 30) {
        analogWrite(pinFans, (int) map(heat, 30, 120, 0, 254));
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


void timer2at250khz() {
    // Pins 11 and 3
    pinMode(11, OUTPUT); // output pin for OCR2B, this is Arduino pin number

    // In the next line of code, we:
    // 1. Set the compare output mode to clear OC2A and OC2B on compare match.
    //    To achieve this, we set bits COM2A1 and COM2B1 to high.
    // 2. Set the waveform generation mode to fast PWM (mode 3 in datasheet).
    //    To achieve this, we set bits WGM21 and WGM20 to high.
    TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);

    // In the next line of code, we:
    // 1. Set the waveform generation mode to fast PWM mode 7 —reset counter on
    //    OCR2A value instead of the default 255. To achieve this, we set bit
    //    WGM22 to high.
    // 2. Set the prescaler divisor to 1, so that our counter will be fed with
    //    the clock's full frequency (16MHz). To achieve this, we set CS20 to
    //    high (and keep CS21 and CS22 to low by not setting them).
    TCCR2B = _BV(WGM22) | _BV(CS20);

    // OCR2A holds the top value of our counter, so it acts as a divisor to the
    // clock. When our counter reaches this, it resets. Counting starts from 0.
    // Thus 63 equals to 64 divs.
    OCR2A = 63;
    // This is the duty cycle. Think of it as the last value of the counter our
    // output will remain high for. Can't be greater than OCR2A of course. A
    // value of 0 means a duty cycle of 1/64 in this case.
    OCR2B = 0;
}

void timer2at150khz() {
    //Timer2 register setup
//Bits 7:6 mean toggle pin 11 on a compare match
//Bits 1:0 and bit 3 in TCCR2B select Fast PWM mode, with OCRA for the TOP value
    TCCR2A = 0b01000011;
//If you want to output on pin 3 instead, set TCCR2A = 0b00010011 instead.
//Bits 2:0 are the prescaler setting, this is a 1 prescaler
    TCCR2B = 0b00001001;
// This is the value that, when the timer reaches it,
// will toggle the output pin and restart. A value of 51 in this case gives a 153.85kHz output
    OCR2A = 51;
}

void timer1FastPWM() {
    // This will activate a PWM frequency of 62500 Hz on the
    // PWM pins associated with Timer1
    // Arduino UNO ==> pin-9 and pin-10
    // Arduino MEGA ==>  pin-11 and pin-12
#if defined(__AVR_ATmega328P__)
    analogWrite(9, 127); // let Arduino do PWM timer and pin initialization
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    analogWrite(11,100); // // let Arduino do PWM timer and pin initialization
#else
  *** wrong board ***
#endif
    // finally set fast-PWM at highest possible frequency
    TCCR1A = _BV(COM1A1) | _BV(WGM10);
    TCCR1B = _BV(CS10) | _BV(WGM12);
}

void timer1Setup() {
// Pins 9 and 10: controlled by timer 1

    // Set PB1 to be an output (Pin9 Arduino UNO)
    DDRB |= (1 << PB1);

    // Clear Timer/Counter Control Registers
    TCCR1A = 0;
    TCCR1B = 0;

    // Set inverting mode
    TCCR1A |= (1 << COM1A1);
    TCCR1A |= (1 << COM1A0);

    // Set PWM Phase Correct, Mode 10
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM13);

    // Set prescaler to 1 and starts PWM
    TCCR1B |= (1 << CS10);
    TCCR1B |= (0 << CS11);

    // Set PWM frequency = 100kHz, duty-cycle = 20%
    ICR1 = (F_CPU / (1 * 200000)) - 1;
    OCR1A = ICR1 / (100 / 20);
}

}


void fastADC() {
// set prescale to 16
    sbi(ADCSRA, ADPS2);
    cbi(ADCSRA, ADPS1);
    cbi(ADCSRA, ADPS0);
}

#endif //POWERSUPPLY_POWERSUPPLY_H
