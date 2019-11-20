
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
