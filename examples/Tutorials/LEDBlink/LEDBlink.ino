/*
   Create a slider widget and connect it to digital pin 9.
   Connect a LED to pin 9.
   The slider controls the blinking frequency of the LED.
*/

#include "RedkeaWiFi.h"

char ssid[] = "xxx";     // your WiFi SSID
char pass[] = "xxx";     // your WiFi password
char deviceID[] = "xxx"; // your device ID

RedkeaWiFi redkea;

int ledPin = 9;
int frequency = 5;
int ledState = LOW;
long long lastChange = millis();

void setup() {
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);
    redkea.begin(ssid, pass, deviceID);
}

void loop() {
    redkea.loop();
    if (millis() - lastChange > 1000 / frequency) {
        ledState = (ledState == LOW) ? HIGH : LOW;
        digitalWrite(ledPin, ledState);
        lastChange = millis();
    }
}

REDKEA_RECEIVE(0) {
    frequency = redkea.readFromSliderWidget(args);
    Serial.println(String("Frequency: ") + frequency);
}