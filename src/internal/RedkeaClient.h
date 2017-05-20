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

#ifndef RedkeaClient_h
#define RedkeaClient_h

#include "RedkeaMessage.h"
#include "RedkeaTimer.h"
#include "RedkeaUserFunctions.h"

template <typename ClientType> class RedkeaClient {
public:
    // pointer to next node in client list
    RedkeaClient* next;

public:
    RedkeaClient(ClientType client, RedkeaList<Sender>* senders, RedkeaList<Receiver>* receivers);

    bool connected();
    void process();
    IPAddress remoteIP();
    void write(const uint8_t* data, uint16_t payloadSize);

private:
    void digitalWrite(const RedkeaMessage& message);
    void analogWrite(const RedkeaMessage& message);
    void functionWrite(const RedkeaMessage& message);

    void digitalRead(Timer* timer);
    void analogRead(Timer* timer);
    void functionRead(Timer* timer);

    void setupTimers(const RedkeaMessage& message);
    void handleTimers();

    void sendPong();

private:
    void handleMessage(const RedkeaMessage& message);

private:
    ClientType m_client;
    RedkeaList<Timer> m_timers;
    RedkeaList<Sender>* m_senders;
    RedkeaList<Receiver>* m_receivers;
};

template <typename ClientType>
RedkeaClient<ClientType>::RedkeaClient(ClientType client, RedkeaList<Sender>* senders, RedkeaList<Receiver>* receivers)
    : next(NULL)
    , m_client(client)
    , m_senders(senders)
    , m_receivers(receivers) {}

template <typename ClientType> IPAddress RedkeaClient<ClientType>::remoteIP() {
    return m_client.remoteIP();
}

template <typename ClientType> void RedkeaClient<ClientType>::handleMessage(const RedkeaMessage& message) {
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

template <typename ClientType> void RedkeaClient<ClientType>::digitalWrite(const RedkeaMessage& message) {
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

template <typename ClientType> void RedkeaClient<ClientType>::analogWrite(const RedkeaMessage& message) {
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

template <typename ClientType> void RedkeaClient<ClientType>::functionWrite(const RedkeaMessage& message) {
    RedkeaMessage::Args it = message.paramsBegin();
    String funIDStr = it.asString();
    Receiver* receiver = m_receivers->head();
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

template <typename ClientType> void RedkeaClient<ClientType>::digitalRead(Timer* timer) {
    uint8_t source = atoi(timer->source.c_str());
    bool value = ::digitalRead(source);
#ifdef REDKEA_DEBUG_VERBOSE
    REDKEA_PRINT_F("Digital read from pin ");
    REDKEA_PRINT(timer->source);
    REDKEA_PRINT_F(": ");
    REDKEA_PRINTLN(value);
#endif
    RedkeaMessage message(RedkeaCommand::DATA_SEND);
    message.addString(timer->widgetID);
    message.addBool(value);
    write(message.data(), message.size());
}

template <typename ClientType> void RedkeaClient<ClientType>::analogRead(Timer* timer) {
    uint8_t source = atoi(timer->source.c_str());
    int16_t value = ::analogRead(source);
#ifdef REDKEA_DEBUG_VERBOSE
    REDKEA_PRINT_F("Analog read from pin ");
    REDKEA_PRINT(timer->source);
    REDKEA_PRINT_F(": ");
    REDKEA_PRINTLN(value);
#endif
    RedkeaMessage message(RedkeaCommand::DATA_SEND);
    message.addString(timer->widgetID);
    message.addInt(value);
    write(message.data(), message.size());
}

template <typename ClientType> void RedkeaClient<ClientType>::functionRead(Timer* timer) {
    Sender* sender = m_senders->head();
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

template <typename ClientType> void RedkeaClient<ClientType>::setupTimers(const RedkeaMessage& message) {
    m_timers.clear();
    RedkeaMessage::Args it = message.paramsBegin();
    uint16_t numTimers = it.asInt();
    for (uint16_t i = 0; i < numTimers; ++i) {
        Timer* timer = new Timer();
        timer->command = (RedkeaCommand)(++it).asByte();
        timer->source = (++it).asString();
        timer->widgetID = (++it).asString();
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

template <typename ClientType> void RedkeaClient<ClientType>::sendPong() {
    REDKEA_PRINTLN_F("Sending pong");
    RedkeaMessage message(RedkeaCommand::PONG);
    write(message.data(), message.size());
}

template <typename ClientType> void RedkeaClient<ClientType>::write(const uint8_t* data, uint16_t payloadSize) {
    m_client.write(data, payloadSize);
}

template <typename ClientType> void RedkeaClient<ClientType>::handleTimers() {
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

template <typename ClientType> bool RedkeaClient<ClientType>::connected() {
    return m_client.connected();
}

template <typename ClientType> void RedkeaClient<ClientType>::process() {
    while (m_client.available()) {
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

#endif