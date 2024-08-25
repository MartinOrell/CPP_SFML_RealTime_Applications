#pragma once

#include "Message.h"
#include "SendMessage.h"
#include <vector>

namespace mrt{

    class CapsuleRunner;

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