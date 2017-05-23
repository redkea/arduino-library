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

template <typename Derived, typename Types> class RedkeaBase {
public:
    RedkeaBase();
    void begin(const String& deviceID);
    void loop();

    void sendToTextWidget(const String& widgetID, const String& text);
    void sendToTextWidget(const String& widgetID, int value);
    void sendToTextWidget(const String& widgetID, float value);

    int readFromSliderWidget(RedkeaMessage::Args args);
    bool readFromToggleWidget(RedkeaMessage::Args args);
    bool readFromTouchWidget(RedkeaMessage::Args args);

    void registerSender(const String& name, RedkeaSendFunctionPtr fun);
    void registerReceiver(const String& name, RedkeaReceiveFunctionPtr fun);

private:
    RedkeaMessage createMessage(const String& widgetID);
    void sendMessage(const RedkeaMessage& message);

private:
    RedkeaLooper<Types> m_looper;
};

template <typename Derived, typename Types>
RedkeaBase<Derived, Types>::RedkeaBase()
    : m_looper(typename Types::ServerType(REDKEA_PORT)) {}

template <typename Derived, typename Types> void RedkeaBase<Derived, Types>::begin(const String& deviceID) {
    m_looper.begin(deviceID);
}

template <typename Derived, typename Types> void RedkeaBase<Derived, Types>::loop() {
    m_looper.loop();
    static_cast<Derived*>(this)->loopOverride();
}

template <typename Derived, typename Types> RedkeaMessage RedkeaBase<Derived, Types>::createMessage(const String& widgetID) {
    RedkeaMessage message(RedkeaCommand::DATA_SEND);
    message.addString(widgetID);
    return message;
}

template <typename Derived, typename Types> void RedkeaBase<Derived, Types>::sendMessage(const RedkeaMessage& message) {
    m_looper.write(message.data(), message.size());
}

template <typename Derived, typename Types> void RedkeaBase<Derived, Types>::sendToTextWidget(const String& widgetID, const String& text) {
    RedkeaMessage message = createMessage(widgetID);
    message.addString(text);
    sendMessage(message);
}

template <typename Derived, typename Types> void RedkeaBase<Derived, Types>::sendToTextWidget(const String& widgetID, int value) {
    RedkeaMessage message = createMessage(widgetID);
    message.addInt(value);
    sendMessage(message);
}

template <typename Derived, typename Types> void RedkeaBase<Derived, Types>::sendToTextWidget(const String& widgetID, float value) {
    RedkeaMessage message = createMessage(widgetID);
    message.addFloat(value);
    sendMessage(message);
}

template <typename Derived, typename Types> int RedkeaBase<Derived, Types>::readFromSliderWidget(RedkeaMessage::Args args) {
    return args.asInt();
}

template <typename Derived, typename Types> bool RedkeaBase<Derived, Types>::readFromToggleWidget(RedkeaMessage::Args args) {
    return args.asBool();
}

template <typename Derived, typename Types> bool RedkeaBase<Derived, Types>::readFromTouchWidget(RedkeaMessage::Args args) {
    return args.asBool();
}

template <typename Derived, typename Types> void RedkeaBase<Derived, Types>::registerSender(const String& name, RedkeaSendFunctionPtr fun) {
    m_looper.registerSender(name, fun);
}

template <typename Derived, typename Types>
void RedkeaBase<Derived, Types>::registerReceiver(const String& name, RedkeaReceiveFunctionPtr fun) {
    m_looper.registerReceiver(name, fun);
}

#endif