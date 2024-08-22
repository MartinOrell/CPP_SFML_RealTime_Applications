#pragma once

#include "Message.h"
#include <stdexcept>

namespace mrt{

    class Capsule{
        public:
            virtual ~Capsule(){}
            virtual int getId() = 0;
            virtual void start() = 0;
            virtual void handleMessage(const Message& message){throw std::invalid_argument("Capsule unable to handle message (missing implementation)");};
            virtual Message handleInvokeMessage(const Message& request){throw std::invalid_argument("Capsule unable to handle invokeMessage (missing implementation)");};
    };

}
