#pragma once

#include "Message.h"
#include <vector>

namespace mrt{

    class CapsuleRunner;
    struct SendMessage;

    class MessageManager{
        public:
            MessageManager(bool allowCrossInvoke = false);
            void addCapsuleRunnerPtr(CapsuleRunner*);
            void replaceCapsuleRunnerPtr(CapsuleRunner* to, CapsuleRunner* from);
            void sendMessage(const SendMessage&);
            void mergeOrSendMessage(const SendMessage&);
            Message invokeMessage(const SendMessage&);

        private:
            bool _allowCrossInvoke;
            std::vector<CapsuleRunner*> _capsuleRunnerPtrs;
    };
    
}