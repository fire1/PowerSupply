
/**
 * Based on "Electronoobs" power supply
 * version $Id$
 */

//#define DEBUG

#include <Arduino.h>


#include "lib/header.h"
#include "lib/functions.h"

// TODO resolve pins



// do not use 10/11
void setup() {
    setupPwm();
#ifdef DEBUG
    Serial.begin(115200);
#else
    lcd.begin(20, 4);
#endif
    pinMode(pinEncoderA, INPUT_PULLUP);
    pinMode(pinEncoderB, INPUT_PULLUP);
    pinMode(pinVolt, INPUT);
    pinMode(pinAmps, INPUT);
    pinMode(pinLed, OUTPUT);
    analogWrite(pinPWM, 0);
    currentMillis = millis();
}
//void loop(){};
void loop() {

    encoder();
    terminal();

    //Why divided by 1.024? Well: I want maximum current of 1000mA. SO 1024 digital read divided by 1000mA =  1.024
    targetAmps = 10;

    // Amperage calculation
    sensAmps();
    sensVolts();


    if ((realVolts > targetVolt + 1 || realVolts < targetVolt - 1)) {
        offset = 0;
        while (realVolts > targetVolt + 1 && realVolts < targetVolt - 1 && offset < 228) {
            sensVolts();
            parsePwm();
            setPwm(pwmValue);
            digitalWrite(pinLed, HIGH);
            delayMicroseconds(10);
            offset++;
        }
    }

    digitalWrite(pinLed, LOW);
    parsePwm();
    setPwm(pwmValue);


    //Each screenRate value we print values on the LCD screen
    currentMillis = millis();
    if (currentMillis - previousMillis >= screenRate) {
        previousMillis += screenRate;
        digitalWrite(pinLed, HIGH);
#ifdef DEBUG
        debug();
#else
        display();
#endif
    }

}//end of void loop
