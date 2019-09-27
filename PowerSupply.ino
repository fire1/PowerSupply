
/**
 * Based on "Electronoobs" power supply
 * version $Id$
 */

//#define DEBUG

#include <Arduino.h>
#include "PowerSupply.h"
#include "lib/PresetMemory.h"
#include "lib/PowerController.h"
#include "lib/DisplayInterface.h"
#include "lib/InputInterface.h"


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
    fastADC();
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
    analogWrite(pinFans, 50);
    delay(400);
    analogWrite(pinFans, 200);
    delay(400);
    analogWrite(pinTone, 220);
    delay(165);
    analogWrite(pinFans, 0);
    analogWrite(pinTone, 0);
}

//
void loop_() {
//    analogWrite(pinFans, 4);
//    return;
//    Serial.println(analogRead(pinAnalogBt));
//    Serial.println(analogRead(pinThermistorLin));
//    OCR2B += 1;
//    if (OCR2B > maxPwmValue) {
//        OCR2B = 0;
//    }
}

uint8_t fanToggle = 0;

void loop() {
    currentLoops = millis();

    in.listen();
    pw.manage();
    if (currentLoops > futureMillis) {
        futureMillis = currentLoops;
        futureMillis += (fastScreen) ? screenEditorRefresh : screenNormalRefresh;
        fanToggle++;

        if (fanToggle > 20) {
            fansControl();
            fanToggle = 0;
        }
        blink();
#ifdef DEBUG
        ui.debug();
#else
        ui.draw();
#endif
    }

}//end of void loop
