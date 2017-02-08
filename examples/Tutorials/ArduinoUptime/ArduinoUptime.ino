/*
   Create a text widget and connect it to user function 0.
   The widget will show the time the Arduino has been running since the last reset.
*/


#include "RedkeaWiFi.h"

char ssid[] = "xxx";     // your WiFi SSID
char pass[] = "xxx";     // your WiFi password
char deviceID[] = "xxx"; // your device ID

RedkeaWiFi redkea;

void setup() {
    Serial.begin(9600);
    redkea.begin(ssid, pass, deviceID);
}

void loop() {
    redkea.loop();
}

REDKEA_SEND(0) {
    long long secondsSinceStart = millis() / 1000;
    String str = "Arduino running since " + secondsSinceStart + " seconds";
    redkea.sendToTextWidget(widgetID, str);
}