
/**
 * Based on "Electronoobs" power supply
 * version $Id$
 */

//#define DEBUG

#include <Arduino.h>
#include "lib/Controller.h"
#include "lib/Interface.h"


#include "PowerSupply.h"

LiquidCrystal lcd(1, 0, 4, 7, 8, 9);
Controller pw;
Interface ui(lcd, pw);
// TODO resolve pins


unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

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

//void loop(){};
void loop() {
    ui.listener();
    pw.manage();
    //Each screenRate value we print values on the LCD screen
    currentMillis = millis();
    if (currentMillis - previousMillis >= screenRate) {
        previousMillis += screenRate;
        digitalWrite(pinLed, HIGH);
#ifdef DEBUG
        ui.debug();
#else
        ui.draw();
#endif
    }

}//end of void loop
