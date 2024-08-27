#pragma once

#include "Message.h"
#include <stdexcept>

namespace mrt{

    class Capsule{
        public:
            virtual ~Capsule(){}
            virtual int getId() = 0;
            virtual void start() = 0;
            virtual void receiveMessage(const Message& message){throw std::invalid_argument("Capsule unable to receive message (missing implementation)");};
            virtual Message receiveInvokeMessage(const Message& request){throw std::invalid_argument("Capsule unable to receive invokeMessage (missing implementation)");};
    };

}
