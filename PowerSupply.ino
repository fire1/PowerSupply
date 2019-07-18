
/**
 * Based on "Electronoobs" power supply
 */

#include <Arduino.h>
#include <LiquidCrystal.h>


// TODO resolve pins
LiquidCrystal lcd(0, 0, 0, 0, 0, 0);

#include <Wire.h>

#include "lib/header.h"
#include "lib/functions.h"


//THIS FUNCTION WILL MAP THE float VALUES IN THE GIVEN RANGE
float fmap(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void timer() {
    //---------------------------------------------- Set PWM frequency for D5 & D6 -------------------------------
    // set timer 0 divisor to     1 for PWM frequency of 62500.00 Hz
    //TCCR0B = TCCR0B & B11111000 | B00000001;
    // set timer 0 divisor to     8 for PWM frequency of  7812.50 Hz
    //TCCR0B = TCCR0B & B11111000 | B00000010;
    // set timer 0 divisor to    64 for PWM frequency of   976.56 Hz (The DEFAULT)
    TCCR0B = TCCR0B & B11111000 | B00000011;
    // set timer 0 divisor to   256 for PWM frequency of   244.14 Hz
    //TCCR0B = TCCR0B & B11111000 | B00000100;
    // set timer 0 divisor to  1024 for PWM frequency of
    //TCCR0B = TCCR0B & B11111000 | B00000101;

    // pin 3 PWM frequency of 31372.55 Hz
    //    TCCR2B = TCCR2B & B11111000 | B00000001;
}


void setup() {
    lcd.begin(16, 4);
    pinMode(pinPwm, INPUT);
    pinMode(pinVolt, INPUT);
    pinMode(pinAmps, INPUT);
    pinMode(currentIn, INPUT);
    pinMode(pinPWM, OUTPUT);
    timer();
    Serial.begin(115200);
    currentMillis = millis();
    //I want maximum voltage of 15V in my case. SO 1024 digital read divided by 15 =  68.2
    mapDivider = 1024.0 / maxVoltage;
}

void loop() {

    encoder();

    //mapDivider = 69.2 in my case Why 68.2? Well: I want maximum voltage of 15V. SO 1024 digital read divided by 15 =  68.2
//    targetVolt = analogRead(pinPwm) / mapDivider;
    terminal((uint8_t)targetVolt);
    //Why divided by 1.024? Well: I want maximum current of 1000mA. SO 1024 digital read divided by 1000mA =  1.024
    targetAmps = analogRead(pinAmps) / 1.024;
    //Read the feedback for current from the MAX471 sensor
    //Scale the ADC, we get current value in Amps
    RawValue = analogRead(currentIn);
    realCurrent = (RawValue * 5.0) / 1024.0;
    //Sub-strack the current error. Make tests in order to find the realAmpOffset value, in my case an error of -0.03A
    realCurrent = realCurrent - realAmpOffset;
    readVolts = analogRead(pinVolt);                           //We read the feedback voltage (0 - 1024)
    realVolts = readVolts / mapDivider;                        //Divide by 69.2 and we get range to 15V

    realCurrentValue = realCurrent * 1000;                           //We pass from A to mA
    realCurrentValue = constrain(realCurrentValue, 0, 2000);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //If the set current value is higher than the feedback current value, we make normal control of output voltage
    if (realCurrentValue < targetAmps) {
        //Now, if set voltage is higher than real value from feedback, we decrease PWM width till we get same value
        if (targetVolt > realVolts) {
            //When we decrease PWM width, the voltage gets higher at the output.
            pwmValue = pwmValue - 1;
            pwmValue = constrain(pwmValue, 0, 255);
        }
        //If set voltage is lower than real value from feedback, we increase PWM width till we get same value
        if (targetVolt < realVolts) {
            //When we increase PWM width, the voltage gets lower at the output.
            pwmValue = pwmValue + 1;
            pwmValue = constrain(pwmValue, 0, 255);
        }
    }//end of realCurrentValue < targetAmps
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*if the set current value is lower than the feedback current value, that means we need to lower the voltage at the output
    in order to amintain the same current value*/
    if (realCurrentValue > targetAmps) {
        //When we increase PWM width, the voltage gets lower at the output.
        pwmValue = pwmValue + 1;
        pwmValue = constrain(pwmValue, 0, 255);
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //We write PWM value on PWM pin out
    analogWrite(pinPWM, pwmValue);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    //Each screenRate value we print values on the LCD screen
    currentMillis = millis();
    if (currentMillis - previousMillis >= screenRate) {
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
