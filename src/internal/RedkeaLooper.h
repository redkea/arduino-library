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
#include "RedkeaMessage.h"
#include "RedkeaUserFunctions.h"
#include "Timer.h"

#include <stdlib.h>

template <typename Types> class RedkeaLooper {
public:
    RedkeaLooper(typename Types::ServerType server);
    void begin(const String& deviceID);
    bool connected();
    void loop();
    void write(const uint8_t* data, uint16_t payloadSize);
    void registerSender(const String& name, RedkeaSendFunctionPtr fun);
    void registerReceiver(const String& name, RedkeaReceiveFunctionPtr fun);

private:
    enum class ConnectionState : uint8_t { NOT_CONNECTED, CONNECTION_LOST, CONNECTED };
    ConnectionState connectionState();

    void handleMessage(const RedkeaMessage& message);

    void digitalWrite(const RedkeaMessage& message);
    void analogWrite(const RedkeaMessage& message);
    void functionWrite(const RedkeaMessage& message);

    void digitalRead(Timer* timer);
    void analogRead(Timer* timer);
    void functionRead(Timer* timer);

    void setupTimers(const RedkeaMessage& message);
    void handleTimers();

    void sendBroadcast();
    void sendPong();

    struct Sender {
        Sender(const String& name, RedkeaSendFunctionPtr fun)
            : name(name)
            , fun(fun)
            , next(NULL) {}
        String name;
        RedkeaSendFunctionPtr fun;
        Sender* next;
    };

    struct Receiver {
        Receiver(const String& name, RedkeaReceiveFunctionPtr fun)
            : name(name)
            , fun(fun)
            , next(NULL) {}
        String name;
        RedkeaReceiveFunctionPtr fun;
        Receiver* next;
    };

    typename Types::ServerType m_server;
    typename Types::ClientType m_client;
    ConnectionState m_connectionState;
    typename Types::UDPType m_udp;
    String m_deviceID;
    RedkeaList<Timer> m_timers;
    RedkeaList<Sender> m_senders;
    RedkeaList<Receiver> m_receivers;
};

template <typename Types>
RedkeaLooper<Types>::RedkeaLooper(typename Types::ServerType server)
    : m_server(server)
    , m_connectionState(ConnectionState::NOT_CONNECTED) {}

template <typename Types> void RedkeaLooper<Types>::begin(const String& deviceID) {
    m_deviceID = deviceID;
    m_server.begin();
    m_udp.begin(REDKEA_DISCOVERY_PORT);
}

template <typename Types> typename RedkeaLooper<Types>::ConnectionState RedkeaLooper<Types>::connectionState() {
    if (m_connectionState == ConnectionState::CONNECTED && !m_client.connected()) {
        m_connectionState = ConnectionState::CONNECTION_LOST;
    }
    return m_connectionState;
}

template <typename Types> bool RedkeaLooper<Types>::connected() {
    return m_connectionState == ConnectionState::CONNECTED;
}

template <typename Types> void RedkeaLooper<Types>::loop() {
    sendBroadcast();

    ConnectionState cs = connectionState();

    // remove disconnected client
    if (cs == ConnectionState::CONNECTION_LOST) {
        m_client = typename Types::ClientType();
        m_connectionState = ConnectionState::NOT_CONNECTED;
        m_timers.clear();
        REDKEA_PRINTLN_F("Lost connection to client");
    }

    // handle new clients
    if (cs == ConnectionState::NOT_CONNECTED) {
        m_client = m_server.available();
        if (!m_client) {
            return;
        }
        m_connectionState = ConnectionState::CONNECTED;
        REDKEA_PRINTLN_F("Accepted connection from client");
    }

    // read data from client
    if (m_client.available()) {
        RedkeaBuffer buffer;

        // read message header
        if (!m_client.readBytes(buffer.data(), sizeof(RedkeaMessage::Header))) {
            REDKEA_PRINTLN_F("Error reading message header");
        }
        buffer.setSize(sizeof(RedkeaMessage::Header));

        uint16_t paramSize = reinterpret_cast<RedkeaMessage::Header*>(buffer.data())->paramSize;
        buffer.increase(sizeof(RedkeaMessage::Header) + paramSize);

        // read message parameters
        if (paramSize > 0) {
            if (m_client.readBytes(buffer.data() + sizeof(RedkeaMessage::Header), paramSize)) {
                buffer.setSize(sizeof(RedkeaMessage::Header) + paramSize);
            } else {
                REDKEA_PRINTLN_F("Error reading message content");
            }
        }
        RedkeaMessage message(buffer);
        handleMessage(message);
    }

    handleTimers();
}

template <typename Types> void RedkeaLooper<Types>::handleMessage(const RedkeaMessage& message) {
    switch (message.command()) {
    case RedkeaCommand::DIGITAL_WRITE: {
        digitalWrite(message);
        break;
    }
    case RedkeaCommand::PWM_WRITE: {
        analogWrite(message);
        break;
    }
#if defined(ARDUINO_SAMD_MKR1000)
    case RedkeaCommand::DAC_WRITE: {
        analogWrite(message);
        break;
    }
#endif
    case RedkeaCommand::FUNCTION_WRITE: {
        functionWrite(message);
        break;
    }
    case RedkeaCommand::WELCOME: {
        REDKEA_PRINTLN_F("Received welcome message");
        break;
    }
    case RedkeaCommand::PING: {
        sendPong();
        break;
    }
    case RedkeaCommand::SETUP_TIMERS: {
        setupTimers(message);
        break;
    }
    default: { break; }
    }
}

template <typename Types> void RedkeaLooper<Types>::digitalWrite(const RedkeaMessage& message) {
    RedkeaMessage::Args it = message.paramsBegin();
    String pinStr = it.asString();
    uint8_t pin = atoi(pinStr.c_str());
    pinMode(pin, OUTPUT);

    RedkeaMessage::Args args = ++it;
    REDKEA_ASSERT(args.isBool());
#ifdef REDKEA_DEBUG_VERBOSE
    REDKEA_PRINT_F("Digital write to pin ");
    REDKEA_PRINT(pin);
    REDKEA_PRINT_F(": ");
    REDKEA_PRINTLN(args.asBool());
#endif
    ::digitalWrite(pin, args.asBool() ? HIGH : LOW);
}

template <typename Types> void RedkeaLooper<Types>::analogWrite(const RedkeaMessage& message) {
    RedkeaMessage::Args it = message.paramsBegin();
    String pinStr = it.asString();
    uint8_t pin = atoi(pinStr.c_str());
    pinMode(pin, OUTPUT);

    RedkeaMessage::Args args = ++it;
    REDKEA_ASSERT(args.isInt());
    REDKEA_ASSERT(digitalPinHasPWM(pin));
#ifdef REDKEA_DEBUG_VERBOSE
    REDKEA_PRINT_F("Analog write to pin ");
    REDKEA_PRINT(pin);
    REDKEA_PRINT_F(": ");
    REDKEA_PRINTLN(args.asInt());
#endif
    ::analogWrite(pin, args.asInt());
}

template <typename Types> void RedkeaLooper<Types>::functionWrite(const RedkeaMessage& message) {
    RedkeaMessage::Args it = message.paramsBegin();
    String funIDStr = it.asString();
    Receiver* receiver = m_receivers.head();
    while (receiver != NULL) {
        if (receiver->name == funIDStr) {
            RedkeaReceiveFunctionPtr fun = receiver->fun;
            fun(++it);
            return;
        }
        receiver = receiver->next;
    }
    REDKEA_PRINT_F("Receiver function ");
    REDKEA_PRINT(funIDStr);
    REDKEA_PRINTLN_F(" is not registered");
}

template <typename Types> void RedkeaLooper<Types>::digitalRead(Timer* timer) {
    uint8_t source = atoi(timer->source.c_str());
    bool value = ::digitalRead(source);
#ifdef REDKEA_DEBUG_VERBOSE
    REDKEA_PRINT_F("Digital read from pin ");
    REDKEA_PRINT(timer->source);
    REDKEA_PRINT_F(": ");
    REDKEA_PRINTLN(value);
#endif
    RedkeaMessage message(RedkeaCommand::DATA_SEND);
    message.addInt(timer->widgetID);
    message.addBool(value);
    write(message.data(), message.size());
}

template <typename Types> void RedkeaLooper<Types>::analogRead(Timer* timer) {
    uint8_t source = atoi(timer->source.c_str());
    int16_t value = ::analogRead(source);
#ifdef REDKEA_DEBUG_VERBOSE
    REDKEA_PRINT_F("Analog read from pin ");
    REDKEA_PRINT(timer->source);
    REDKEA_PRINT_F(": ");
    REDKEA_PRINTLN(value);
#endif
    RedkeaMessage message(RedkeaCommand::DATA_SEND);
    message.addInt(timer->widgetID);
    message.addInt(value);
    write(message.data(), message.size());
}

template <typename Types> void RedkeaLooper<Types>::functionRead(Timer* timer) {
    Sender* sender = m_senders.head();
    while (sender != NULL) {
        if (sender->name == timer->source) {
            RedkeaSendFunctionPtr fun = sender->fun;
            fun(timer->widgetID);
            return;
        }
        sender = sender->next;
    }
    REDKEA_PRINT_F("Sender function ");
    REDKEA_PRINT(timer->source);
    REDKEA_PRINTLN_F(" is not registered");
}

template <typename Types> void RedkeaLooper<Types>::setupTimers(const RedkeaMessage& message) {
    m_timers.clear();
    RedkeaMessage::Args it = message.paramsBegin();
    uint16_t numTimers = it.asInt();
    for (uint16_t i = 0; i < numTimers; ++i) {
        Timer* timer = new Timer();
        timer->command = (RedkeaCommand)(++it).asByte();
        timer->source = (++it).asString();
        timer->widgetID = (++it).asInt();
        timer->interval = (++it).asInt();
        timer->lastCall = millis();
#ifdef REDKEA_DEBUG_VERBOSE
        REDKEA_PRINTLN_F("New timer");
        REDKEA_PRINT_F("\tCommand: ");
        REDKEA_PRINTLN((uint8_t)timer->command);
        REDKEA_PRINT_F("\tSource: ");
        REDKEA_PRINTLN(timer->source);
        REDKEA_PRINT_F("\tWidgetID: ");
        REDKEA_PRINTLN(timer->widgetID);
        REDKEA_PRINT_F("\tInterval: ");
        REDKEA_PRINTLN(timer->interval);
#endif
        m_timers.add(timer);
    }
}

template <typename Types> void RedkeaLooper<Types>::sendPong() {
    REDKEA_PRINTLN_F("Sending pong");
    RedkeaMessage message(RedkeaCommand::PONG);
    write(message.data(), message.size());
}

template <typename Types> void RedkeaLooper<Types>::write(const uint8_t* data, uint16_t payloadSize) {
    m_client.write(data, payloadSize);
}

template <typename Types> void RedkeaLooper<Types>::handleTimers() {
    Timer* timer = m_timers.head();
    while (timer != NULL) {
        long long timeSinceCall = millis() - timer->lastCall;
        if (timeSinceCall >= timer->interval) {
            switch (timer->command) {
            case RedkeaCommand::DIGITAL_READ: {
                digitalRead(timer);
                break;
            }
            case RedkeaCommand::ADC_READ: {
                analogRead(timer);
                break;
            }
            case RedkeaCommand::FUNCTION_READ: {
                functionRead(timer);
                break;
            }
            }
            timer->lastCall = millis();
        }
        timer = timer->next;
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

#endif
