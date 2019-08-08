
/**
 * Based on "Electronoobs" power supply
 * version $Id$
 */

//#define DEBUG

#include <Arduino.h>
#include "lib/Controller.h"
#include "lib/UserInterface.h"
#include "PowerSupply.h"

// https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home
LiquidCrystal lcd(1, 0, 4, 7, 8, 9);

RotaryEncoder ec(pinEncoderA, pinEncoderB);
AnalogButtons ab(pinAnalogBt, INPUT, 10);
Controller pw;
UserInterface ui(lcd, pw, ec, ab);


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
    ui.listener();
    pw.manage();
    //Each screenRate value we print values on the LCD screen
//    currentLoops = millis();
    currentLoops++;
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
