#pragma once

#include "SendMessage.h"
#include "Timer.h"
#include <mutex>
#include <vector>
#include <optional>

namespace mrt{

    class MessageHandler{
        public:
            MessageHandler();
            MessageHandler(MessageHandler&&);
            void addTimer(const Timer&);
            void removeTimer(int timerId);
            void sendMessage(const SendMessage&);
            void mergeOrSendMessage(const SendMessage&);
            std::optional<SendMessage> receiveMessage();
        private:

            std::mutex _mutex;
            std::vector<SendMessage> _messageStack;
            std::vector<Timer> _timers;
            std::timed_mutex _waitForMessageMutex;
    };
    
}