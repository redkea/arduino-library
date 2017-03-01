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

#ifndef RedkeaUserFunctions_h
#define RedkeaUserFunctions_h

#include "RedkeaMessage.h"

typedef void(*RedkeaReceiveFunctionPtr)(RedkeaMessage::Args args);

template <typename BaseType>
struct RedkeaRegisterReceiver {
    RedkeaRegisterReceiver<BaseType>(BaseType* base, const String& name, RedkeaReceiveFunctionPtr ptr) {
        base->registerReceiver(name, ptr);
    }
};

#define REDKEA_RECEIVER(name, args) \
    void name (RedkeaMessage::Args args)

#define REDKEA_REGISTER_RECEIVER(obj, name) \
    RedkeaRegisterReceiver<decltype(obj)> registerReceiver_ ## name(&obj, #name, &name);



typedef void(*RedkeaSendFunctionPtr)(const String& widgetID);

template <typename BaseType>
struct RedkeaRegisterSender {
    RedkeaRegisterSender<BaseType>(BaseType* base, const String& name, RedkeaSendFunctionPtr ptr) {
        base->registerSender(name, ptr);
    }
};

#define REDKEA_SENDER(name, widgetID) \
    void name (const String& widgetID)

#define REDKEA_REGISTER_SENDER(obj, name) \
    RedkeaRegisterSender<decltype(obj)> registerSender_ ## name(&obj, #name, &name);

#endif