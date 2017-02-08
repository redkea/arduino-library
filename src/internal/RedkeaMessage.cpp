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

#include "RedkeaMessage.h"

RedkeaBuffer::RedkeaBuffer()
    : m_capacity(REDKEA_BUFFER_STACK_CAPACITY)
    , m_size(0)
    , m_data{ .begin = NULL } {}

RedkeaBuffer::RedkeaBuffer(const RedkeaBuffer& other)
    : m_capacity(other.m_capacity)
    , m_size(other.m_size) {
    if (other.m_capacity <= REDKEA_BUFFER_STACK_CAPACITY) {
        memcpy(m_data.local, other.m_data.local, REDKEA_BUFFER_STACK_CAPACITY);
    }
    else {
        m_data.begin = new uint8_t[other.m_capacity];
        memcpy(m_data.begin, other.m_data.begin, other.m_size);
    }
}

RedkeaBuffer::~RedkeaBuffer() {
    if (m_capacity > REDKEA_BUFFER_STACK_CAPACITY) {
        delete m_data.begin;
    }
}

void RedkeaBuffer::increase(uint16_t capacity) {
    if (m_size + capacity <= m_capacity || m_size + capacity <= REDKEA_BUFFER_STACK_CAPACITY) {
        return;
    }

    uint16_t newCapacity = m_size + 2 * capacity;
    uint8_t* newData = new uint8_t[newCapacity];
    if (m_size > 0) {
        if (m_capacity <= REDKEA_BUFFER_STACK_CAPACITY) {
            memcpy(newData, m_data.local, REDKEA_BUFFER_STACK_CAPACITY);
        }
        else {
            memcpy(newData, m_data.begin, m_size);
        }
    }
    if (m_capacity > REDKEA_BUFFER_STACK_CAPACITY) {
        delete m_data.begin;
    }
    m_data.begin = newData;
    m_capacity = newCapacity;
}

uint16_t RedkeaBuffer::size() const {
    return m_size;
}

void RedkeaBuffer::setSize(uint16_t size) {
    m_size = size;
}

const uint8_t* RedkeaBuffer::data() const {
    if (m_capacity <= REDKEA_BUFFER_STACK_CAPACITY) {
        return m_data.local;
    }
    return m_data.begin;
}

uint8_t* RedkeaBuffer::data() {
    return const_cast<uint8_t*>(static_cast<const RedkeaBuffer*>(this)->data());
}

void RedkeaBuffer::append(const uint8_t* buf, int16_t size) {
    increase(size);
    memcpy(&(data()[m_size]), buf, size);
    m_size += size;
}

RedkeaMessage::Args::Args(const uint8_t* begin)
    : m_begin(begin) {}

RedkeaDataType RedkeaMessage::Args::dataType() const {
    return static_cast<RedkeaDataType>(m_begin[0]);
}

bool RedkeaMessage::Args::isByte() const {
    return static_cast<RedkeaDataType>(m_begin[0]) == RedkeaDataType::UINT8;
}

bool RedkeaMessage::Args::isInt() const {
    return static_cast<RedkeaDataType>(m_begin[0]) == RedkeaDataType::INT16;
}

bool RedkeaMessage::Args::isBool() const {
    return static_cast<RedkeaDataType>(m_begin[0]) == RedkeaDataType::BOOLEAN;
}

bool RedkeaMessage::Args::isFloat() const {
    return static_cast<RedkeaDataType>(m_begin[0]) == RedkeaDataType::FLOAT;
}

bool RedkeaMessage::Args::isString() const {
    return static_cast<RedkeaDataType>(m_begin[0]) == RedkeaDataType::STRING;
}

uint8_t RedkeaMessage::Args::asByte() const {
    REDKEA_ASSERT(dataType() == RedkeaDataType::UINT8);
    uint8_t value;
    memcpy(&value, &m_begin[1], sizeof(uint8_t));
    return value;
}

int16_t RedkeaMessage::Args::asInt() const {
    REDKEA_ASSERT(dataType() == RedkeaDataType::INT16);
    int16_t value;
    memcpy(&value, &m_begin[1], sizeof(int16_t));
    return value;
}

bool RedkeaMessage::Args::asBool() const {
    REDKEA_ASSERT(dataType() == RedkeaDataType::BOOLEAN);
    return (m_begin[1] & 0x01) != 0;
}

float RedkeaMessage::Args::asFloat() const {
    REDKEA_ASSERT(dataType() == RedkeaDataType::FLOAT);
    float value;
    memcpy(&value, &m_begin[1], sizeof(float));
    return value;
}

String RedkeaMessage::Args::asString() const {
    REDKEA_ASSERT(dataType() == RedkeaDataType::STRING);
    int16_t size;
    memcpy(&size, &m_begin[1], sizeof(int16_t));
    String str;
    for (int16_t i = 0; i < size; ++i) {
        str += (char)m_begin[i + sizeof(uint8_t) + sizeof(int16_t)];
    }
    return str;
}

RedkeaMessage::Args& RedkeaMessage::Args::operator++() {
    switch (dataType()) {
    case RedkeaDataType::UINT8: {
        m_begin += (sizeof(uint8_t) + sizeof(uint8_t));
        break;
    }
    case RedkeaDataType::INT16: {
        m_begin += (sizeof(uint8_t) + sizeof(int16_t));
        break;
    }
    case RedkeaDataType::BOOLEAN: {
        m_begin += (sizeof(uint8_t) + sizeof(uint8_t));
        break;
    }
    case RedkeaDataType::FLOAT: {
        m_begin += (sizeof(uint8_t) + sizeof(float));
        break;
    }
    case RedkeaDataType::STRING: {
        m_begin += (sizeof(uint8_t) + sizeof(int16_t) + m_begin[1]);
        break;
    }
    default: {
        REDKEA_ASSERT(false);
        break;
    }
    }
    return *this;
}

bool operator==(const RedkeaMessage::Args& lhs, const RedkeaMessage::Args& rhs) {
    return lhs.m_begin == rhs.m_begin;
}

bool operator!=(const RedkeaMessage::Args& lhs, const RedkeaMessage::Args& rhs) {
    return !(lhs == rhs);
}

RedkeaMessage::RedkeaMessage(RedkeaCommand command) {
    Header header;
    header.command = command;
    header.paramSize = 0;
    m_buffer.append(header);
}

RedkeaMessage::RedkeaMessage(RedkeaBuffer buffer)
    : m_buffer(buffer) {}

RedkeaCommand RedkeaMessage::command() const {
    const Header* header = reinterpret_cast<const Header*>(m_buffer.data());
    return header->command;
}

const uint8_t* RedkeaMessage::data() const {
    return m_buffer.data();
}

uint16_t RedkeaMessage::size() const {
    return m_buffer.size();
}

RedkeaMessage::Args RedkeaMessage::paramsBegin() const {
    return Args(m_buffer.data() + sizeof(Header));
}

RedkeaMessage::Args RedkeaMessage::paramsEnd() const {
    return Args(m_buffer.data() + m_buffer.size());
}

void RedkeaMessage::addByte(uint8_t value) {
    m_buffer.append(static_cast<uint8_t>(RedkeaDataType::UINT8));
    m_buffer.append(value);
    Header* header = reinterpret_cast<Header*>(m_buffer.data());
    header->paramSize += 2;
}

void RedkeaMessage::addInt(int16_t value) {
    m_buffer.append(static_cast<uint8_t>(RedkeaDataType::INT16));
    m_buffer.append(value);
    Header* header = reinterpret_cast<Header*>(m_buffer.data());
    header->paramSize += 3;
}

void RedkeaMessage::addBool(bool value) {
    m_buffer.append(static_cast<uint8_t>(RedkeaDataType::BOOLEAN));
    m_buffer.append(value ? static_cast<uint8_t>(0x01) : static_cast<uint8_t>(0x00));
    Header* header = reinterpret_cast<Header*>(m_buffer.data());
    header->paramSize += 2;
}

void RedkeaMessage::addFloat(float value) {
    m_buffer.append(static_cast<uint8_t>(RedkeaDataType::FLOAT));
    m_buffer.append(value);
    Header* header = reinterpret_cast<Header*>(m_buffer.data());
    header->paramSize += 5;
}

void RedkeaMessage::addString(const char* value, uint16_t size) {
    m_buffer.append(static_cast<uint8_t>(RedkeaDataType::STRING));
    m_buffer.append(size);
    m_buffer.append((const uint8_t*)value, size);
    Header* header = reinterpret_cast<Header*>(m_buffer.data());
    header->paramSize += 3 + size;
}

void RedkeaMessage::addString(const String& value) {
    addString(value.c_str(), value.length());
}