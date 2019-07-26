
/**
 * Based on "Electronoobs" power supply
 */

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <PID_v1.h>

#ifndef PID_v1_h

#include "../libraries/PID/PID_v1.h"

#endif


#include "lib/header.h"
#include "lib/functions.h"

// TODO resolve pins
LiquidCrystal lcd(0, 1, 2, 9, 7, 8);


// do not use 10/11
void setup() {
//    lcd.begin(16, 4);
//    pinMode(pinPwm, INPUT);
    pinMode(pinVolt, INPUT);
    pinMode(pinAmps, INPUT);
    Serial.begin(115200);
    currentMillis = millis();
    //I want maximum voltage of 15V in my case. SO 1024 digital read divided by 15 =  68.2
    mapDividerVolt = 1024.0 / maxVoltage;
    setupPwm();
}

void loop() {

    encoder();
    terminal(targetVolt);

    targetAmps = 3;

    //Why divided by 1.024? Well: I want maximum current of 1000mA. SO 1024 digital read divided by 1000mA =  1.024
    targetAmps = 18;
    //Read the feedback for current from the MAX471 sensor
    //Scale the ADC, we get current value in Amps
//

    //
    // Amperage calculation
    for (index = 0; index < 4; ++index) {
        avrReadAmps += analogRead(pinAmps);
    }
    dumpAmps = readAmps = avrReadAmps = avrReadAmps / 4;
    if (readAmps > 232) {
        realCurrent = map(readAmps, 232, 550, 900, 5300);
    } else {
        realCurrent = map(readAmps, 30, 232, 55, 900);
    }
    realCurrent = realCurrent < 0 ? 0 : realCurrent / 1000;
    realCurrentValue = realCurrent;                           //We pass from A to mA




    //Sub-strack the current error. Make tests in order to find the realAmpOffset value, in my case an error of -0.03A
//    realCurrent = realCurrent - realAmpOffset;
    dumpVolts = readVolts = analogRead(pinVolt);
//    realVolts = readVolts / mapDividerVolt;                        //Divide by 69.2 and we get range to 15V
    realVolts = fmap(readVolts, 11, 379, 0.86, 10.5);                        //Divide by 69.2 and we get range to 15V

//    realCurrentValue = constrain(realCurrentValue, 0, 800);


/*    double gap = abs(targetVolt - realVolts); //distance away from setpoint
    if (gap < 0.5) {  //we're close to setpoint, use conservative tuning parameters
        myPID.SetTunings(consKp, consKi, consKd);
    } else {
        //we're far from setpoint, use aggressive tuning parameters
        myPID.SetTunings(aggKp, aggKi, aggKd);
    }*/
//    myPID.Compute();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //If the set current value is higher than the feedback current value, we make normal control of output voltage
    if (realCurrentValue < targetAmps) {
        //Now, if set voltage is higher than real value from feedback, we decrease PWM width till we get same value
        if (targetVolt > realVolts) {
            //When we decrease PWM width, the voltage gets higher at the output.
            pwmValue = pwmValue - 1;
            pwmValue = constrain(pwmValue, 0, maxPwmControl);
        }
        //If set voltage is lower than real value from feedback, we increase PWM width till we get same value
        if (targetVolt < realVolts) {
            //When we increase PWM width, the voltage gets lower at the output.
            pwmValue = pwmValue + 1;
            pwmValue = constrain(pwmValue, 0, maxPwmControl);
        }
    }//end of realCurrentValue < targetAmps
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*if the set current value is lower than the feedback current value, that means we need to lower the voltage at the output
    in order to amintain the same current value*/
    if (realCurrentValue > targetAmps) {
        //When we increase PWM width, the voltage gets lower at the output.
        pwmValue = pwmValue + 1;
        pwmValue = constrain(pwmValue, 0, maxPwmControl);
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //We write PWM value on PWM pin out
    setPwm(pwmValue);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    //Each screenRate value we print values on the LCD screen
    currentMillis = millis();
    if (currentMillis - previousMillis >= screenRate) {
        debug();
        previousMillis += screenRate;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" VOLTAGE    CURRENT ");

        lcd.setCursor(0, 1);
        lcd.print(targetVolt, 1);
        lcd.print("V       ");

        lcd.print(targetAmps, 0);
        lcd.print("mA");

        lcd.setCursor(0, 3);
        lcd.print(realVolts, 1);
        lcd.print("V       ");

        lcd.print(realCurrentValue, 0);
        //lcd.setCursor(19,1);
        lcd.print("mA");
    }//end of currentMillis - previousMillis >= screenRate

}//end of void loop
