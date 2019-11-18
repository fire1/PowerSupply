
/**
 * Based on "Electronoobs" power supply
 * version $Id$
 */

//#define DEBUG


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
    pw.measure();
}


// do not use 10/11
void setup() {
    fastADC();
    pw.begin();

    Serial.begin(115200);

    lcd.begin(20, 4);

    in.begin();
    ui.begin();
    currentLoops = 0;

    pinMode(pinFans, OUTPUT);
    pinMode(pinTone, OUTPUT);
    analogWrite(pinFans, 50);
    delay(400);
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
    noAlarm();
    in.listen();
    if (is10)
        pw.manage();

    if (currentLoops > futureMillis) {
        futureMillis = currentLoops;
        futureMillis += (fastScreen) ? screenEditorRefresh : screenNormalRefresh;
        fanToggle++;

        if (fanToggle > 20) {
            fansControl();
            fanToggle = 0;
        }
        blink();
        ui.debug();
        ui.draw();

    }

}//end of void loop
