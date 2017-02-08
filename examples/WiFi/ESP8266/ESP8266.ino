#include "RedkeaESP8266.h"

char ssid[] = "xxx";     // your WiFi SSID
char pass[] = "xxx";     // your WiFi password
char deviceID[] = "xxx"; // your device ID

RedkeaESP8266 redkea;

void setup() {
    Serial.begin(9600);
    redkea.begin(ssid, pass, deviceID);
}

void loop() {
    redkea.loop();
}