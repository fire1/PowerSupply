
/**
 * Based on "Electronoobs" power supply
 * version $Id$
 */

// TODO
// EEP ROM, Shutdown mode (when edit), Battery charge mode
//#define DEBUG

#include <Arduino.h>
#include "lib/PowerController.h"
#include "lib/DisplayInterface.h"
#include "lib/InputInterface.h"
#include "PowerSupply.h"


// https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home
LiquidCrystal lcd(1, 0, 4, 7, 8, 9);

RotaryEncoder ec(pinEncoderA, pinEncoderB);
AnalogButtons ab(pinAnalogBt, INPUT, 10);
PowerController pw;
InputInterface in(pw, ec, ab);
DisplayInterface ui(lcd, pw, in);


void interruptFunction() {
    ec.tick();
}


// do not use 10/11
void setup() {

    pw.begin();
#ifdef DEBUG
    Serial.begin(115200);
#else
    lcd.begin(20, 4);
#endif
    in.begin();
    ui.begin();
    currentLoops = 0;


    pinMode(pinFanA, OUTPUT);
    pinMode(pinFanB, OUTPUT);
    analogWrite(pinFanA, 0);
    analogWrite(pinFanB, 0);
}

//
void loop_() {
    analogWrite(pinFanA, 100);
}

void loop() {
    currentLoops++;
    in.listen();
    pw.manage();
    if (currentLoops - previousMillis >= screenRefresh) {
        previousMillis += screenRefresh;
        fansControl();
        digitalWrite(pinLed, HIGH);
#ifdef DEBUG
        ui.debug();
#else
        ui.draw();
#endif
    }

}//end of void loop
