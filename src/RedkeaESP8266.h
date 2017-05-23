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
				<label for="deviceID">Device ID</label>
				<input name="deviceID" value="">
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
    void begin();
    void begin(const char* ssid, const char* pass, const char* deviceID);
    void loopOverride();

private:
    bool hasWiFiSetup();
    String readSettingsAndConnect();
    void connectToWiFi(const char* ssid, const char* pass);
    void createWiFiAP();
    void handleRoot();
    void handleSetup();

private:
    ESP8266WebServer* webserver;
};

void RedkeaESP8266::begin() {
    EEPROM.begin(512);

    pinMode(D1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(D1), resetMemory, FALLING);

    String deviceID;
    if (hasWiFiSetup()) {
        deviceID = readSettingsAndConnect();
    } else {
        createWiFiAP();
    }

    RedkeaBase::begin(deviceID);
}

void RedkeaESP8266::begin(const char* ssid, const char* pass, const char* deviceID) {
    connectToWiFi(ssid, pass);
    RedkeaBase::begin(deviceID);
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

String RedkeaESP8266::readSettingsAndConnect() {
    String ssid;
    int ssidOffset = 1;
    uint8_t ssidSize = EEPROM.read(ssidOffset);
    for (uint8_t i = 0; i < ssidSize; ++i) {
        ssid += (char)EEPROM.read(ssidOffset + 1 + i);
    }

    String pass;
    int passOffset = ssidOffset + 1 + ssidSize;
    uint8_t passSize = EEPROM.read(passOffset);
    for (uint8_t i = 0; i < passSize; ++i) {
        pass += (char)EEPROM.read(passOffset + 1 + i);
    }

    String deviceID;
    int deviceIDOffset = passOffset + 1 + passSize;
    uint8_t deviceIDSize = EEPROM.read(deviceIDOffset);
    for (uint8_t i = 0; i < deviceIDSize; ++i) {
        deviceID += (char)EEPROM.read(deviceIDOffset + 1 + i);
    }

    REDKEA_PRINTLN(ssid);
    REDKEA_PRINTLN(pass);
    REDKEA_PRINTLN(deviceID);

    connectToWiFi(ssid.c_str(), pass.c_str());

    return deviceID;
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
    WiFi.mode(WIFI_AP);
    String mac = WiFi.macAddress();
    String apName = "RedkeaAP_";
    apName += mac[0];
    apName += mac[1];
    apName += mac[3];
    apName += mac[4];
    REDKEA_PRINT_F("Create WiFi AP: ");
    REDKEA_PRINTLN_F(apName);
    if (!WiFi.softAP(apName.c_str(), "RedkeaAP")) {
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
    String deviceID = webserver->arg("deviceID");
    REDKEA_PRINTLN_F("Received WiFi setup:");
    REDKEA_PRINT_F("SSID: ");
    REDKEA_PRINTLN(ssid);
    REDKEA_PRINT_F("Password: ");
    REDKEA_PRINTLN(pass);
    REDKEA_PRINT_F("Device ID: ");
    REDKEA_PRINTLN(deviceID);

    EEPROM.write(0, 1);
    int ssidOffset = 1;
    EEPROM.write(ssidOffset, ssid.length());
    for (int i = 0; i < ssid.length(); ++i) {
        EEPROM.write(ssidOffset + 1 + i, ssid[i]);
    }
    int passOffset = ssidOffset + 1 + ssid.length();
    EEPROM.write(passOffset, pass.length());
    for (int i = 0; i < pass.length(); ++i) {
        EEPROM.write(passOffset + 1 + i, pass[i]);
    }
    int deviceIDOffset = passOffset + 1 + pass.length();
    EEPROM.write(deviceIDOffset, deviceID.length());
    for (int i = 0; i < deviceID.length(); ++i) {
        EEPROM.write(deviceIDOffset + 1 + i, deviceID[i]);
    }
    EEPROM.commit();


    webserver->send(200, "text/html", setupHtml);
    delay(2000);
    ESP.restart();
}

#endif
