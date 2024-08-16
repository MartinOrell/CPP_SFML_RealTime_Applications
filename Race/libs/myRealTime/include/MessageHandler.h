#pragma once

#include <mutex>
#include <vector>
#include <algorithm>
#include <optional>
#include "SendMessage.h"

class MessageHandler{
    public:
        MessageHandler();
        MessageHandler(MessageHandler&&);
        void addTimer(Timer);
        void removeTimer(int timerId);
        void sendMessage(SendMessage);
        void mergeOrSendMessage(SendMessage);
        std::optional<SendMessage> receiveMessage();
    private:

        std::mutex _mutex;
        std::vector<SendMessage> _messageStack;
        std::vector<Timer> _timers;
        std::timed_mutex _waitForMessageMutex;
};