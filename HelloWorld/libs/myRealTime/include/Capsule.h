#pragma once

#include "Message.h"
#include <stdexcept>

namespace mrt{

    class Capsule{
        public:
            virtual ~Capsule(){}
            virtual int getId() = 0;
            virtual void start() = 0;
            virtual void handleMessage(Message message){throw std::invalid_argument("Capsule unable to handle message (missing implementation)");};
            virtual Message handleInvokeMessage(Message request){throw std::invalid_argument("Capsule unable to handle invokeMessage (missing implementation)");};
    };

}
