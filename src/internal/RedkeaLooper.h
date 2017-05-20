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

#ifndef RedkeaLooper_h
#define RedkeaLooper_h

#include "RedkeaLooper.h"
#include "RedkeaClient.h"
#include "RedkeaUserFunctions.h"

#include <stdlib.h>

template <typename Types> class RedkeaLooper {
public:
    RedkeaLooper(typename Types::ServerType server);
    void begin(const String& deviceID);
    void loop();
    void registerSender(const String& name, RedkeaSendFunctionPtr fun);
    void registerReceiver(const String& name, RedkeaReceiveFunctionPtr fun);
    void write(const uint8_t* data, uint16_t payloadSize);

private:
    void sendBroadcast();

private:
    typename Types::ServerType m_server;
    RedkeaList<RedkeaClient<typename Types::ClientType>> m_clients;
    typename Types::UDPType m_udp;
    String m_deviceID;
    RedkeaList<Sender> m_senders;
    RedkeaList<Receiver> m_receivers;
};

template <typename Types>
RedkeaLooper<Types>::RedkeaLooper(typename Types::ServerType server)
    : m_server(server) {}

template <typename Types> void RedkeaLooper<Types>::begin(const String& deviceID) {
    m_deviceID = deviceID;
    m_server.begin();
    m_udp.begin(REDKEA_DISCOVERY_PORT);
}

template <typename Types> void RedkeaLooper<Types>::loop() {
    sendBroadcast();

    typename Types::ClientType client = m_server.available();
    if (client) {
#ifdef ESP8266
        bool newClient = true;
        RedkeaClient<typename Types::ClientType>* it = m_clients.head();
        while (it != NULL) {
            if (it->remoteIP() == client.remoteIP()) {
                newClient = false;
                break;
            }
            it = it->next;
        }

        if (newClient) {
            REDKEA_PRINT_F("Accepted client connection: ");
            REDKEA_PRINTLN(client.remoteIP());
            m_clients.add(new RedkeaClient<typename Types::ClientType>(client, &m_senders, &m_receivers));
        }
#else
        if (m_clients.head() == NULL) {
            m_clients.add(new RedkeaClient<typename Types::ClientType>(client, &m_senders, &m_receivers));
        }
#endif
    }

    RedkeaClient<typename Types::ClientType>* it = m_clients.head();
    while (it != NULL) {
        if (it->connected()) {
            it->process();
            it = it->next;
        }
        else {
            REDKEA_PRINTLN_F("Client has disconnected");
            it = m_clients.remove(it);
        }
    }
}

template <typename Types> void RedkeaLooper<Types>::sendBroadcast() {
    int packetSize = m_udp.parsePacket();
    if (packetSize > 0) {
        char buffer[128];
        int bytesRead = m_udp.read(buffer, packetSize);
        if (!strncmp(buffer, "REDKEA_DISCOVERY_REQ", 20)) {
            IPAddress remoteIP = m_udp.remoteIP();
            int remotePort = m_udp.remotePort();
            m_udp.beginPacket(remoteIP, remotePort);
            const char replyHeader[] = "REDKEA_DISCOVERY_REP";
            String reply = replyHeader + m_deviceID;
            m_udp.write((uint8_t*)reply.c_str(), reply.length());
            m_udp.endPacket();
        }
    }
}

template <typename Types> void RedkeaLooper<Types>::registerSender(const String& name, RedkeaSendFunctionPtr fun) {
    Sender* sender = new Sender(name, fun);
    m_senders.add(sender);
}

template <typename Types> void RedkeaLooper<Types>::registerReceiver(const String& name, RedkeaReceiveFunctionPtr fun) {
    Receiver* receiver = new Receiver(name, fun);
    m_receivers.add(receiver);
}

template <typename Types>
void RedkeaLooper<Types>::write(const uint8_t* data, uint16_t payloadSize)
{
    RedkeaClient<typename Types::ClientType>* it = m_clients.head();
    while (it != NULL) {
        it->write(data, payloadSize);
        it = it->next;
    }
}
#endif
