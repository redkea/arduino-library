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

#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

#include "internal/RedkeaBase.h"

struct Types {
    typedef WiFiServer ServerType;
    typedef WiFiClient ClientType;
    typedef WiFiUDP UDPType;
};

const char rootHtml[] PROGMEM =
R"(<html><body>
				<h1>Redkea WiFi Setup</h1>
				<form action="http://192.168.4.1/setup" method="post">
				  <div>
					<label for="ssid">WiFi SSID</label>
					<input name="ssid" value="Redkea123">
				  </div>
				  <div>
					<label for="pass">WiFi Password</label>
					<input name="pass" value="Redkea123">
				  </div>
				  <div>
					<button>Save Configuration</button>
				  </div>
				</form>
			</body></html>
	)";

const char setupHtml[] PROGMEM = R"(
		<html>
			<body>
				<h1>WiFi Configuration Saved. Rebooting.</h1>
			</body>
		</html>
	)";

void resetMemory() {
    REDKEA_PRINTLN_F("Reset EEPROM");
    for (int i = 0; i < 512; ++i) {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
    delay(2000);
    ESP.restart();
}

class RedkeaESP8266 : public RedkeaBase<RedkeaESP8266, Types> {
public:
    void begin(const char* deviceID);
    void begin(const char* ssid, const char* pass, const char* deviceID);
    void loopOverride();

private:
    bool hasWiFiSetup();
    void readSettingsAndConnect();
    void connectToWiFi(const char* ssid, const char* pass);
    void createWiFiAP();
    void handleRoot();
    void handleSetup();

private:
    ESP8266WebServer* webserver;
};

void RedkeaESP8266::begin(const char* deviceID) {
    EEPROM.begin(512);

    pinMode(D1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(D1), resetMemory, FALLING);

    if (hasWiFiSetup()) {
        readSettingsAndConnect();
    } else {
        createWiFiAP();
    }

    RedkeaBase::begin(deviceID);
}

void RedkeaESP8266::begin(const char* ssid, const char* pass, const char* deviceID) {
    connectToWiFi(ssid, pass);
}

void RedkeaESP8266::loopOverride() {
    if (webserver != nullptr) {
        webserver->handleClient();
    }
}

bool RedkeaESP8266::hasWiFiSetup() {
    REDKEA_PRINTLN(EEPROM.read(0));
    return EEPROM.read(0);
}

void RedkeaESP8266::readSettingsAndConnect() {
    String ssid;
    uint8_t ssidSize = EEPROM.read(1);
    for (uint8_t i = 0; i < ssidSize; ++i) {
        ssid += (char)EEPROM.read(2 + i);
    }

    String pass;
    uint8_t passSize = EEPROM.read(3 + ssidSize);
    for (uint8_t i = 0; i < passSize; ++i) {
        pass += (char)EEPROM.read(3 + ssidSize + i);
    }

    connectToWiFi(ssid.c_str(), pass.c_str());
}

void RedkeaESP8266::connectToWiFi(const char* ssid, const char* pass) {
    REDKEA_PRINT_F("Attempting to connect to SSID: ");
    REDKEA_PRINTLN(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    REDKEA_PRINT_F("Connected. IP: ");
    REDKEA_PRINTLN(WiFi.localIP());
}

void RedkeaESP8266::createWiFiAP() {
    REDKEA_PRINTLN_F("Create WiFi AP");
    WiFi.mode(WIFI_AP);
    if (!WiFi.softAP("RedkeaAP", "RedkeaAP")) {
        REDKEA_PRINTLN_F("Error creating WiFi AP");
    }
    IPAddress myIP = WiFi.softAPIP();
    REDKEA_PRINT_F("AP IP address: ");
    REDKEA_PRINTLN(myIP);

    webserver = new ESP8266WebServer(80);
    webserver->on("/", std::bind(&RedkeaESP8266::handleRoot, this));
    webserver->on("/setup", std::bind(&RedkeaESP8266::handleSetup, this));
    webserver->begin();
}

void RedkeaESP8266::handleRoot() {
    
    webserver->send(200, "text/html", rootHtml);
}

void RedkeaESP8266::handleSetup() {
    String ssid = webserver->arg("ssid");
    String pass = webserver->arg("pass");
    REDKEA_PRINTLN_F("Received WiFi setup:");
    REDKEA_PRINT_F("SSID: ");
    REDKEA_PRINTLN(ssid);
    REDKEA_PRINT_F("Password: ");
    REDKEA_PRINTLN(pass);

    EEPROM.write(0, 1);
    EEPROM.write(1, ssid.length());
    for (int i = 0; i < ssid.length(); ++i) {
        EEPROM.write(2 + i, ssid[i]);
    }
    EEPROM.write(pass.length(), 3 + ssid.length());
    for (int i = 0; i < pass.length(); ++i) {
        EEPROM.write(3 + ssid.length() + i, pass[i]);
    }
    EEPROM.commit();


    webserver->send(200, "text/html", setupHtml);
    delay(2000);
    ESP.restart();
}

#endif
