
/**
 * Based on "Electronoobs" power supply
 * version $Id$
 *
 *
 *  TODO attach int to am limiting in and update dislay more frq-tly
 *  Check unlocked control
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
PowerController pc;
InputInterface in(pc, ec, ab, pm);
DisplayInterface ui(lcd, pc, in);
//

void encoderInterrupt() {
    ec.tick();
}


void inaAlertInterrupt() {
//    pc.measure();
    cntMsr++;
}

void ampLimitInterrupt() {
    ampLimiter =  (uint8_t) (analogRead(A0) / 10);
    if (ampLimiter < 30) {
        digitalWrite(pinLed, HIGH);
    }
}


// do not use 10/11
void setup() {
    Serial.begin(115200);
    lcd.begin(20, 4);
    ui.begin();

    tick();
    ina.begin(4, 28100);
    ina.setBusConversion(8500);            // Maximum conversion time 8.244ms
    ina.setShuntConversion(8500);          // Maximum conversion time 8.244ms
    ina.setMode(INA_MODE_CONTINUOUS_BOTH); // Bus/shunt measured continuously
    Serial.println(F("Booting setPowerState supply ..."));
    delay(300);
    pinMode(pinFans, OUTPUT);
    pinMode(pinTone, OUTPUT);
    pc.begin();


    in.begin();
    currentTime = 0;

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
    currentTime = millis();
    noAlarm();
    in.listen();
    pc.manage();

    //
    // Update bar
    if (is20() && pc.isLimited()) ui.drawBar(pc.getLimit());

    if (currentTime > futureMillis) {
        pc.calculate();
        futureMillis = currentTime;
        futureMillis += (fastScreen) ? screenEditorRefresh : screenNormalRefresh;
        fanToggle++;

        if (fanToggle > 20) {
            fansControl();
            fanToggle = 0;
        }
        ui.debug();
        digitalWrite(pinLed, LOW);
        ui.draw();
    }

}//end of void loop
