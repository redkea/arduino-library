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

#define REDKEA_REGISTER_RECEIVER(obj, name) \
    obj.registerReceiver(#name, &name);

#define REDKEA_RECEIVER(name) \
    void name (RedkeaMessage::Args args)

typedef void(*RedkeaSendFunctionPtr)(const char widgetID[UID_LENGTH]);

#define REDKEA_SENDER(name) \
    void name (const char widgetID[UID_LENGTH])

#define REDKEA_REGISTER_SENDER(obj, name) \
    obj.registerSender(#name, &name);

#endif