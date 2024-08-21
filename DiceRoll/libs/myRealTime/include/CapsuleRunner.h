#pragma once

#include <variant>
#include <cassert>
#include <memory>
#include "Message.h"
#include "SendMessage.h"
#include "MessageHandler.h"
#include "Capsule.h"

namespace mrt{

    class MessageManager;

    class CapsuleRunner{
        public:
            CapsuleRunner(int id, MessageManager* messageManagerPtr);
            CapsuleRunner(CapsuleRunner&&);
            void addCapsule(std::unique_ptr<Capsule>);
            bool isResponsible(int id);
            void run();
            void stop();
            void sendMessage(SendMessage message);
            void mergeOrSendMessage(SendMessage);
            Message invokeMessage(SendMessage request);
            int informIn(int, std::chrono::steady_clock::duration);
            int informEvery(int, std::chrono::steady_clock::duration);
            void cancelTimer(int);
        private:
            bool handleMessage(SendMessage message);
            bool handleMessageToMe(Message message);

            int _id;
            MessageHandler _messageHandler;
            MessageManager* _messageManagerPtr;
            std::vector<std::unique_ptr<Capsule>> _capsules;
        public:
            int _nextTimerId;
    };

}