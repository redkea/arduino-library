/*
        redkea library for Arduino
        Copyright 2017 redkea

        This file is part of the redkea library for Arduino.

    The redkea library for Arduino is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The redkea library for Arduino is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the redkea library for Arduino.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RedkeaESP8266_h
#define RedkeaESP8266_h

#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

#include "internal/RedkeaBase.h"

struct Types {
    typedef WiFiServer ServerType;
    typedef WiFiClient ClientType;
    typedef WiFiUDP UDPType;
};

class RedkeaESP8266 : public RedkeaBase<Types> {
public:
    void begin(const char* ssid, const char* pass, const char* deviceID);
};

void RedkeaESP8266::begin(const char* ssid, const char* pass, const char* deviceID) {
    REDKEA_PRINT_F("Attempting to connect to SSID: ");
    REDKEA_PRINTLN(ssid);

    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    REDKEA_PRINT_F("Connected. IP: ");
    REDKEA_PRINTLN(WiFi.localIP());

    RedkeaBase::begin(deviceID);
}

#endif
