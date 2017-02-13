/*
   Create a text widget and connect it to user function 0.
   The widget will show the time the Arduino has been running since the last reset.
*/


#include "RedkeaWiFi.h"

char ssid[] = "xxx";     // your WiFi SSID
char pass[] = "xxx";     // your WiFi password
char deviceID[] = "xxx"; // your device ID

RedkeaWiFi redkea;

REDKEA_SENDER(sendUptime) {
    long secondsSinceStart = millis() / 1000;
    String str = "Arduino running since ";
    str += secondsSinceStart;
    str += " seconds";
    redkea.sendToTextWidget(widgetID, str);
}

void setup() {
    Serial.begin(9600);
    redkea.begin(ssid, pass, deviceID);

    REDKEA_REGISTER_SENDER(redkea, sendUptime);
}

void loop() {
    redkea.loop();
}
