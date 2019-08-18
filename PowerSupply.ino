
/**
 * Based on "Electronoobs" power supply
 * version $Id$
 */

//#define DEBUG

#include <Arduino.h>
#include "lib/PresetMemory.h"
#include "lib/PowerController.h"
#include "lib/DisplayInterface.h"
#include "lib/InputInterface.h"
#include "PowerSupply.h"


// https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home
LiquidCrystal lcd(1, 0, 4, 7, 8, 9);

RotaryEncoder ec(pinEncoderA, pinEncoderB);
AnalogButtons ab(pinAnalogBt, INPUT, 10);
PresetMemory pm;
PowerController pw;
InputInterface in(pw, ec, ab, pm);
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


    pinMode(pinFans, OUTPUT);
    pinMode(pinTone, OUTPUT);

    analogWrite(pinFans, 200);
    delay(24500);
    tone(pinTone, 2200);
    delay(16500);
    analogWrite(pinFans, 0);
    noTone(pinTone);
}

//
void loop_() {
    analogWrite(pinFans, 254);
//    Serial.println(analogRead(pinAnalogBt));
}

void loop() {
    currentLoops++;
    in.listen();
    pw.manage();
    if (currentLoops > previousMillis) {
        previousMillis = currentLoops;
        previousMillis += (fastScreen) ? screenEditorRefresh : screenNormalRefresh;
        fansControl();
        digitalWrite(pinLed, HIGH);
#ifdef DEBUG
        ui.debug();
#else
        ui.draw();
#endif
    }

}//end of void loop
