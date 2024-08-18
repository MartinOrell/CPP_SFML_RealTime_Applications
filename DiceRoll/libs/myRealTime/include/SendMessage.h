#pragma once

#include "Message.h"
#include <variant>

struct SendMessage{
    int toId;
    Message message;
    bool operator==(const SendMessage& rhs);
    void merge(SendMessage fromM);
};