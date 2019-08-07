
/**
 * Based on "Electronoobs" power supply
 * version $Id$
 */

#define DEBUG

#include <Arduino.h>

#include "lib/Controller.h"
#include "lib/UserInterface.h"


#include "PowerSupply.h"


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
    currentMillis = millis();
}
//
void loop_() {
    analogWrite(pinFanA, 100);
}

void loop() {
    ui.listener();
    pw.manage();
    //Each screenRate value we print values on the LCD screen
//    currentMillis = millis();
    currentMillis++;
    if (currentMillis - previousMillis >= screenRefresh) {
        previousMillis += screenRefresh;

        int rawTemp = analogRead(pinThermistor);
        heat = map(rawTemp, 345, 460, 120, 70);

        if (heat > 25) {
            analogWrite(pinFanB, map(heat, 80, 120, 0, 255));
            if (heat > 35) {
                analogWrite(pinFanA, map(heat, 0, 120, 0, 255));
            }
        } else if (heat < 22) {
            analogWrite(pinFanA, map(heat, 80, 120, 0, 0));
            analogWrite(pinFanB, map(heat, 0, 120, 0, 0));
        }
        digitalWrite(pinLed, HIGH);
#ifdef DEBUG
        ui.debug();
#else
        ui.draw();
#endif
    }

}//end of void loop
