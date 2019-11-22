
/**
 * Based on "Electronoobs" power supply
 * version $Id$
 */

//#define DEBUG

int cntMsr;
#include <Arduino.h>
#include "PowerSupply.h"
#include "lib/PresetMemory.h"
#include "lib/PowerController.h"
#include "lib/DisplayInterface.h"
#include "lib/InputInterface.h"


// https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home


RotaryEncoder ec(pinEncoderA, pinEncoderB);
AnalogButtons ab(pinAnalogBt, INPUT, 10);
PresetMemory pm;
PowerController pw;
InputInterface in(pw, ec, ab, pm);
DisplayInterface ui(lcd, pw, in);
//

void encoderInterrupt() {
    ec.tick();
}



void inaAlertInterrupt() {
//    pw.measure();
    cntMsr++;
}


// do not use 10/11
void setup() {
    Serial.begin(115200);
    Serial.println(F("Booting power supply ..."));
    tick();
 /*   if (!ina.begin()) Serial.println(F("No ina found ..."));
    ina.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
    ina.calibrate(0.01, 3.0);*/
    ina.begin(4,28100);
    ina.setBusConversion(8500);            // Maximum conversion time 8.244ms
    ina.setShuntConversion(8500);          // Maximum conversion time 8.244ms
//    ina.setAveraging(32);                 // Average each reading n-times
    ina.setMode(INA_MODE_CONTINUOUS_BOTH); // Bus/shunt measured continuously


    delay(500);
    pinMode(pinFans, OUTPUT);
    pinMode(pinTone, OUTPUT);
//    fastADC();
    ui.begin();
    pw.begin();


    lcd.begin(20, 4);

    in.begin();
    currentLoops = 0;

    analogWrite(pinFans, 50);
    tick();
    delay(400);
    noAlarm();
    analogWrite(pinFans, 200);
    delay(400);
    analogWrite(pinTone, 220);
    delay(165);
    analogWrite(pinFans, 0);
    analogWrite(pinTone, 0);

}


uint8_t fanToggle = 0;

void loop() {
    currentLoops = millis();
    digitalWrite(pinLed, LOW);
    noAlarm();
    in.listen();
    if (is80())
        pw.manage();

    if (currentLoops > futureMillis) {
        pw.calculate();
        futureMillis = currentLoops;
        futureMillis += (fastScreen) ? screenEditorRefresh : screenNormalRefresh;
        fanToggle++;

        if (fanToggle > 20) {
            fansControl();
            fanToggle = 0;
        }
        ui.debug();
        ui.draw();

    }

}//end of void loop
