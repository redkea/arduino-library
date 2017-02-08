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

#ifndef RedkeaMessage_h
#define RedkeaMessage_h

#include "RedkeaUtil.h"

#define REDKEA_BUFFER_STACK_CAPACITY 32

// Buffer for sending and receiving messages.
// For messages smaller than REDKEA_BUFFER_STACK_CAPACITY, no heap memory is allocated.
class RedkeaBuffer {
public:
    RedkeaBuffer();
    ~RedkeaBuffer();
    RedkeaBuffer(const RedkeaBuffer& other);

    void increase(uint16_t capacity);

    uint16_t size() const;
    void setSize(uint16_t size);

    const uint8_t* data() const;
    uint8_t* data();

    template <typename T> void append(const T& value);
	void append(const uint8_t* buf, int16_t size);

private:
    uint16_t m_capacity;
    uint16_t m_size;
    union {
        uint8_t* begin;
        uint8_t local[REDKEA_BUFFER_STACK_CAPACITY];
    } m_data;
};

template <typename T> void RedkeaBuffer::append(const T& value) {
    increase(sizeof(T));
    memcpy(&(data()[m_size]), &value, sizeof(T));
    m_size += sizeof(T);
}

enum class RedkeaCommand : uint8_t {
    // app to device commands
    WRITE_TO_DIGITAL_PIN = 0,
    WRITE_TO_ANALOG_PIN = 1,
    WRITE_TO_FUNCTION = 2,
    WELCOME = 3,
    SETUP_TIMERS = 4,
    PING = 5,

    // in-device commands
    READ_FROM_DIGITAL_PIN = 100,
    READ_FROM_ANALOG_PIN = 101,
    READ_FROM_FUNCTION = 102,

    // device to app commands
    DATA_SEND = 200,
    PONG = 201
};

enum class RedkeaDataType : uint8_t { UINT8 = 0, INT16 = 1, BOOLEAN = 2, FLOAT = 3, STRING = 4 };

class RedkeaMessage {
public:
    struct Header {
        RedkeaCommand command;
        uint16_t paramSize;
    } __attribute__((__packed__));

    class Args {
    public:
        Args(const uint8_t* begin);

        RedkeaDataType dataType() const;

        bool isByte() const;
        bool isInt() const;
        bool isBool() const;
		bool isFloat() const;
		bool isString() const;

        uint8_t asByte() const;
        int16_t asInt() const;
        bool asBool() const;
		float asFloat() const;
		String asString() const;

        Args& operator++();

        friend bool operator==(const Args& lhs, const Args& rhs);
        friend bool operator!=(const Args& lhs, const Args& rhs);

    private:
        const uint8_t* m_begin;
    };

    explicit RedkeaMessage(RedkeaCommand command);
    explicit RedkeaMessage(RedkeaBuffer buffer);

    RedkeaCommand command() const;
    const uint8_t* data() const;
    uint16_t size() const;

    Args paramsBegin() const;
    Args paramsEnd() const;
    
    void addByte(uint8_t value);
    void addInt(int16_t value);
    void addBool(bool value);
	void addFloat(float value);
    void addString(const char* value, uint16_t size);
	void addString(const String& value);

private:
    RedkeaBuffer m_buffer;
};

#endif