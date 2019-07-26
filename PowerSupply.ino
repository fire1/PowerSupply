
/**
 * Based on "Electronoobs" power supply
 * version $Id$
 */

#include <Arduino.h>



#include "lib/header.h"
#include "lib/functions.h"

// TODO resolve pins



// do not use 10/11
void setup() {
//    lcd.begin(16, 4);
//    pinMode(pinPwm, INPUT);
    pinMode(pinVolt, INPUT);
    pinMode(pinAmps, INPUT);
    Serial.begin(115200);
    currentMillis = millis();
    setupPwm();
}

void loop() {

    encoder();
    terminal(targetVolt);

    //Why divided by 1.024? Well: I want maximum current of 1000mA. SO 1024 digital read divided by 1000mA =  1.024
    targetAmps = 10;

    // Amperage calculation
    sensAmps();
    sensVolts();

    if ((realVolts > targetVolt + 1 && realVolts < targetVolt - 1))
        while (realVolts > targetVolt + 1 && realVolts < targetVolt - 1) {
            sensVolts();
            parse();
            setPwm(pwmValue);
        }

    setPwm(pwmValue);


    //Each screenRate value we print values on the LCD screen
    currentMillis = millis();
    if (currentMillis - previousMillis >= screenRate) {
        previousMillis += screenRate;
        debug();
        display();
    }

}//end of void loop
