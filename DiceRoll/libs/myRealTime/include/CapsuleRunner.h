#pragma once

#include <memory>
#include <vector>
#include "Message.h"
#include "MessageHandler.h"

namespace mrt{

    class Capsule;
    class MessageManager;
    struct SendMessage;

    class CapsuleRunner{
        public:
            CapsuleRunner(int id, MessageManager* messageManagerPtr);
            CapsuleRunner(CapsuleRunner&&);
            void addCapsule(std::unique_ptr<Capsule>);
            bool isResponsible(int id);
            void run();
            void stop();
            void sendMessage(const SendMessage& message);
            void mergeOrSendMessage(const SendMessage&);
            Message invokeMessage(const SendMessage& request);
            int informIn(int, std::chrono::steady_clock::duration);
            int informEvery(int, std::chrono::steady_clock::duration);
            void cancelTimer(int);
        private:
            bool handleMessage(const SendMessage& message);
            bool handleMessageToMe(const Message& message);

            int _id;
            MessageHandler _messageHandler;
            MessageManager* _messageManagerPtr;
            std::vector<std::unique_ptr<Capsule>> _capsules;
        public:
            int _nextTimerId;
    };

}