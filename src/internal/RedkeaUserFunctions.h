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

#define REDKEA_DECLARE_FUNCTION_RECEIVE(id) \
     void __attribute__((weak)) redkeaReceive ## id(RedkeaMessage::Args args);

#define REDKEA_RECEIVE(id) \
    void redkeaReceive ## id(RedkeaMessage::Args args)

#define REDKEA_DECLARE_FUNCTION_SEND(id) \
     void __attribute__((weak)) redkeaSend ## id(const char widgetID[UID_LENGTH]);

#define REDKEA_SEND(id) \
    void redkeaSend ## id(const char widgetID[UID_LENGTH])

/*
* RECEIVE FUNCTIONS
**/

REDKEA_DECLARE_FUNCTION_RECEIVE(0)
REDKEA_DECLARE_FUNCTION_RECEIVE(1)
REDKEA_DECLARE_FUNCTION_RECEIVE(2)
REDKEA_DECLARE_FUNCTION_RECEIVE(3)
REDKEA_DECLARE_FUNCTION_RECEIVE(4)
REDKEA_DECLARE_FUNCTION_RECEIVE(5)
REDKEA_DECLARE_FUNCTION_RECEIVE(6)
REDKEA_DECLARE_FUNCTION_RECEIVE(7)
REDKEA_DECLARE_FUNCTION_RECEIVE(8)
REDKEA_DECLARE_FUNCTION_RECEIVE(9)

REDKEA_DECLARE_FUNCTION_RECEIVE(10)
REDKEA_DECLARE_FUNCTION_RECEIVE(11)
REDKEA_DECLARE_FUNCTION_RECEIVE(12)
REDKEA_DECLARE_FUNCTION_RECEIVE(13)
REDKEA_DECLARE_FUNCTION_RECEIVE(14)
REDKEA_DECLARE_FUNCTION_RECEIVE(15)
REDKEA_DECLARE_FUNCTION_RECEIVE(16)
REDKEA_DECLARE_FUNCTION_RECEIVE(17)
REDKEA_DECLARE_FUNCTION_RECEIVE(18)
REDKEA_DECLARE_FUNCTION_RECEIVE(19)

REDKEA_DECLARE_FUNCTION_RECEIVE(20)
REDKEA_DECLARE_FUNCTION_RECEIVE(21)
REDKEA_DECLARE_FUNCTION_RECEIVE(22)
REDKEA_DECLARE_FUNCTION_RECEIVE(23)
REDKEA_DECLARE_FUNCTION_RECEIVE(24)
REDKEA_DECLARE_FUNCTION_RECEIVE(25)
REDKEA_DECLARE_FUNCTION_RECEIVE(26)
REDKEA_DECLARE_FUNCTION_RECEIVE(27)
REDKEA_DECLARE_FUNCTION_RECEIVE(28)
REDKEA_DECLARE_FUNCTION_RECEIVE(29)

REDKEA_DECLARE_FUNCTION_RECEIVE(30)
REDKEA_DECLARE_FUNCTION_RECEIVE(31)
REDKEA_DECLARE_FUNCTION_RECEIVE(32)
REDKEA_DECLARE_FUNCTION_RECEIVE(33)
REDKEA_DECLARE_FUNCTION_RECEIVE(34)
REDKEA_DECLARE_FUNCTION_RECEIVE(35)
REDKEA_DECLARE_FUNCTION_RECEIVE(36)
REDKEA_DECLARE_FUNCTION_RECEIVE(37)
REDKEA_DECLARE_FUNCTION_RECEIVE(38)
REDKEA_DECLARE_FUNCTION_RECEIVE(39)

typedef void(*RedkeaReceiveFunctionPtr)(RedkeaMessage::Args args);
const RedkeaReceiveFunctionPtr redkeaReceiveFunctionArray[]{
    &redkeaReceive0, 
    &redkeaReceive1, 
    &redkeaReceive2, 
    &redkeaReceive3, 
    &redkeaReceive4,
    &redkeaReceive5,
    &redkeaReceive6,
    &redkeaReceive7,
    &redkeaReceive8,
    &redkeaReceive9,
    &redkeaReceive10,
    &redkeaReceive11,
    &redkeaReceive12,
    &redkeaReceive13,
    &redkeaReceive14,
    &redkeaReceive15,
    &redkeaReceive16,
    &redkeaReceive17,
    &redkeaReceive18,
    &redkeaReceive19,
    &redkeaReceive20,
    &redkeaReceive21,
    &redkeaReceive22,
    &redkeaReceive23,
    &redkeaReceive24,
    &redkeaReceive25,
    &redkeaReceive26,
    &redkeaReceive27,
    &redkeaReceive28,
    &redkeaReceive29,
    &redkeaReceive30,
    &redkeaReceive31,
    &redkeaReceive32,
    &redkeaReceive33,
    &redkeaReceive34,
    &redkeaReceive35,
    &redkeaReceive36,
    &redkeaReceive37,
    &redkeaReceive38,
    &redkeaReceive39,
};

/*
* SEND FUNCTIONS
**/

REDKEA_DECLARE_FUNCTION_SEND(0)
REDKEA_DECLARE_FUNCTION_SEND(1)
REDKEA_DECLARE_FUNCTION_SEND(2)
REDKEA_DECLARE_FUNCTION_SEND(3)
REDKEA_DECLARE_FUNCTION_SEND(4)
REDKEA_DECLARE_FUNCTION_SEND(5)
REDKEA_DECLARE_FUNCTION_SEND(6)
REDKEA_DECLARE_FUNCTION_SEND(7)
REDKEA_DECLARE_FUNCTION_SEND(8)
REDKEA_DECLARE_FUNCTION_SEND(9)

REDKEA_DECLARE_FUNCTION_SEND(10)
REDKEA_DECLARE_FUNCTION_SEND(11)
REDKEA_DECLARE_FUNCTION_SEND(12)
REDKEA_DECLARE_FUNCTION_SEND(13)
REDKEA_DECLARE_FUNCTION_SEND(14)
REDKEA_DECLARE_FUNCTION_SEND(15)
REDKEA_DECLARE_FUNCTION_SEND(16)
REDKEA_DECLARE_FUNCTION_SEND(17)
REDKEA_DECLARE_FUNCTION_SEND(18)
REDKEA_DECLARE_FUNCTION_SEND(19)

REDKEA_DECLARE_FUNCTION_SEND(20)
REDKEA_DECLARE_FUNCTION_SEND(21)
REDKEA_DECLARE_FUNCTION_SEND(22)
REDKEA_DECLARE_FUNCTION_SEND(23)
REDKEA_DECLARE_FUNCTION_SEND(24)
REDKEA_DECLARE_FUNCTION_SEND(25)
REDKEA_DECLARE_FUNCTION_SEND(26)
REDKEA_DECLARE_FUNCTION_SEND(27)
REDKEA_DECLARE_FUNCTION_SEND(28)
REDKEA_DECLARE_FUNCTION_SEND(29)

REDKEA_DECLARE_FUNCTION_SEND(30)
REDKEA_DECLARE_FUNCTION_SEND(31)
REDKEA_DECLARE_FUNCTION_SEND(32)
REDKEA_DECLARE_FUNCTION_SEND(33)
REDKEA_DECLARE_FUNCTION_SEND(34)
REDKEA_DECLARE_FUNCTION_SEND(35)
REDKEA_DECLARE_FUNCTION_SEND(36)
REDKEA_DECLARE_FUNCTION_SEND(37)
REDKEA_DECLARE_FUNCTION_SEND(38)
REDKEA_DECLARE_FUNCTION_SEND(39)

typedef void(*RedkeaSendFunctionPtr)(const char widgetID[UID_LENGTH]);
const RedkeaSendFunctionPtr redkeaSendFunctionArray[]{
    &redkeaSend0,
    &redkeaSend1,
    &redkeaSend2,
    &redkeaSend3,
    &redkeaSend4,
    &redkeaSend5,
    &redkeaSend6,
    &redkeaSend7,
    &redkeaSend8,
    &redkeaSend9,
    &redkeaSend10,
    &redkeaSend11,
    &redkeaSend12,
    &redkeaSend13,
    &redkeaSend14,
    &redkeaSend15,
    &redkeaSend16,
    &redkeaSend17,
    &redkeaSend18,
    &redkeaSend19,
    &redkeaSend20,
    &redkeaSend21,
    &redkeaSend22,
    &redkeaSend23,
    &redkeaSend24,
    &redkeaSend25,
    &redkeaSend26,
    &redkeaSend27,
    &redkeaSend28,
    &redkeaSend29,
    &redkeaSend30,
    &redkeaSend31,
    &redkeaSend32,
    &redkeaSend33,
    &redkeaSend34,
    &redkeaSend35,
    &redkeaSend36,
    &redkeaSend37,
    &redkeaSend38,
    &redkeaSend39,
};

#endif