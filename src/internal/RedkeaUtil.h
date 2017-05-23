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

#ifndef RedkeaUtil_h
#define RedkeaUtil_h

#include "RedkeaConfig.h"

#define REDKEA_FATAL(message) Serial.println(message);


#if defined(__AVR__)
#include <avr/pgmspace.h>
#define REDKEA_F(arg) F(arg)
#else
#define REDKEA_F(arg) arg
#endif


#ifdef REDKEA_DEBUG
#define REDKEA_PRINTLN(message) (Serial.println(message));
#define REDKEA_PRINT(message) (Serial.print(message));
#define REDKEA_PRINTLN_F(message) (Serial.println(REDKEA_F(message)));
#define REDKEA_PRINT_F(message) (Serial.print(REDKEA_F(message)));
#define REDKEA_ASSERT(cond)                                                                                                                \
    if (!(cond)) {                                                                                                                         \
        Serial.println(String(REDKEA_F(#cond)) + REDKEA_F(" (") + __FILE__ + REDKEA_F(":") + __LINE__ + REDKEA_F(")"));                    \
    }
#else
#define REDKEA_PRINTLN(message)
#define REDKEA_PRINT(message)
#define REDKEA_PRINTLN_F(message)
#define REDKEA_PRINT_F(message)
#define REDKEA_ASSERT(cond)
#endif


template <typename Node> class RedkeaList {
public:
    RedkeaList()
        : m_head(NULL) {}
    ~RedkeaList() { clear(); }

    Node* head() { return m_head; }

    void add(Node* node) {
        if (m_head == NULL) {
            m_head = node;
        } else {
            Node* it = m_head;
            while (it->next != NULL) {
                it = it->next;
            }
            it->next = node;
        }
        node->next = NULL;
    }

    Node* remove(Node* node) {
        if (m_head == node) {
            m_head = node->next;
            delete node;
            return m_head;
        } else {
            Node* it = m_head;
            while (it->next != node) {
                it = it->next;
            }
            it->next = node->next;
            delete node;
            return it->next;
        }
    }

    void clear() {
        Node* current = m_head;
        while (current != NULL) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        m_head = NULL;
    }

private:
    Node* m_head;
};

#endif