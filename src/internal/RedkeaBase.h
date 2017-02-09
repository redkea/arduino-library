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

#ifndef RedkeaBase_h
#define RedkeaBase_h

#include "RedkeaLooper.h"
#include "RedkeaMessage.h"
#include "RedkeaUtil.h"

template <typename Types> class RedkeaBase {
public:
    RedkeaBase();
    void begin(const String& deviceID);
    void loop();

    void sendToTextWidget(const char widgetID[UID_LENGTH], const String& text);
    void sendToTextWidget(const char widgetID[UID_LENGTH], int value);
    void sendToTextWidget(const char widgetID[UID_LENGTH], float value);

    int readFromSliderWidget(RedkeaMessage::Args args);
    bool readFromToggleWidget(RedkeaMessage::Args args);
    bool readFromTouchWidget(RedkeaMessage::Args args);

    void registerSender(const String& name, RedkeaSendFunctionPtr fun);
    void registerReceiver(const String& name, RedkeaReceiveFunctionPtr fun);

private:
    RedkeaMessage createMessage(const char widgetID[UID_LENGTH]);
    void sendMessage(const RedkeaMessage& message);

private:
    RedkeaLooper<Types> m_looper;
};

template <typename Types>
RedkeaBase<Types>::RedkeaBase()
    : m_looper(typename Types::ServerType(REDKEA_PORT)) {}

template <typename Types> void RedkeaBase<Types>::begin(const String& deviceID) {
    m_looper.begin(deviceID);
}

template <typename Types> void RedkeaBase<Types>::loop() {
    m_looper.loop();
}

template <typename Types> RedkeaMessage RedkeaBase<Types>::createMessage(const char widgetID[UID_LENGTH]) {
    RedkeaMessage message(RedkeaCommand::DATA_SEND);
    message.addString(widgetID, UID_LENGTH);
    return message;
}

template <typename Types> void RedkeaBase<Types>::sendMessage(const RedkeaMessage& message) {
    m_looper.write(message.data(), message.size());
}

template <typename Types> void RedkeaBase<Types>::sendToTextWidget(const char widgetID[UID_LENGTH], const String& text) {
    RedkeaMessage message = createMessage(widgetID);
    message.addString(text);
    sendMessage(message);
}

template <typename Types> void RedkeaBase<Types>::sendToTextWidget(const char widgetID[UID_LENGTH], int value) {
    RedkeaMessage message = createMessage(widgetID);
    message.addInt(value);
    sendMessage(message);
}

template <typename Types> void RedkeaBase<Types>::sendToTextWidget(const char widgetID[UID_LENGTH], float value) {
    RedkeaMessage message = createMessage(widgetID);
    message.addFloat(value);
    sendMessage(message);
}

template <typename Types> int RedkeaBase<Types>::readFromSliderWidget(RedkeaMessage::Args args) {
    return args.asInt();
}

template <typename Types> bool RedkeaBase<Types>::readFromToggleWidget(RedkeaMessage::Args args) {
    return args.asBool();
}

template <typename Types> bool RedkeaBase<Types>::readFromTouchWidget(RedkeaMessage::Args args) {
    return args.asBool();
}

template <typename Types> void RedkeaBase<Types>::registerSender(const String& name, RedkeaSendFunctionPtr fun) {
    m_looper.registerSender(name, fun);
}

template <typename Types> void RedkeaBase<Types>::registerReceiver(const String& name, RedkeaReceiveFunctionPtr fun) {
    m_looper.registerReceiver(name, fun);
}

#endif