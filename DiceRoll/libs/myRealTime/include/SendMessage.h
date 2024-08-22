#pragma once

#include "Message.h"

namespace mrt{

    struct SendMessage{
        int toId;
        Message message;
        bool operator==(const SendMessage& rhs);
        void merge(SendMessage fromM);
    };
    
}