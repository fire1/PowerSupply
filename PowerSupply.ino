
/**
 * Based on "Electronoobs" power supply
 * version $Id$
 */

#define DEBUG

#include <Arduino.h>
#include "lib/Controller.h"
#include "lib/UserInterface.h"


#include "PowerSupply.h"
#include "lib/AnalogButtons.h"


LiquidCrystal lcd(1, 0, 4, 7, 8, 9);

RotaryEncoder ec(pinEncoderA, pinEncoderB);
AnalogButtons bt;
Controller pw;
UserInterface ui(lcd, pw, ec, bt);


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
    bt.begin();
    currentMillis = millis();
}

//void loop(){};
void loop() {
    ui.listener();
    pw.manage();
    //Each screenRate value we print values on the LCD screen
    currentMillis = millis();
    bt.listener();
    if (currentMillis - previousMillis >= screenRefresh) {
        previousMillis += screenRefresh;

        int rawTemp = analogRead(pinThermistor);
        heat = map(rawTemp, 345, 460, 120, 70);

        analogWrite(pinFanA, map(heat, 50, 120, 0, 255));
        analogWrite(pinFanB, map(heat, 0, 120, 0, 255));
        digitalWrite(pinLed, HIGH);
#ifdef DEBUG
        ui.debug();
#else
        ui.draw();
#endif
    }

}//end of void loop
