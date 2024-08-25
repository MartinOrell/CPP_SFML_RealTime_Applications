#pragma once

#include <memory>
#include <vector>
#include "Message.h"
#include "SendMessage.h"
#include "MessageHandler.h"

namespace mrt{

    class Capsule;
    class MessageManager;

    class CapsuleRunner{
        public:
            CapsuleRunner(int id, MessageManager* messageManagerPtr = nullptr);
            CapsuleRunner(CapsuleRunner&&);
            void addCapsule(std::unique_ptr<Capsule>);
            bool isResponsible(int id);
            void run();
            void stop();
            void sendMessage(const SendMessage&);
            void mergeOrSendMessage(const SendMessage&);
            Message invokeMessage(const SendMessage&);
            int informIn(int toId, std::chrono::steady_clock::duration duration);
            int informEvery(int toId, std::chrono::steady_clock::duration interval);
            void cancelTimer(int timerId);
        private:
            bool handleMessage(const SendMessage&);
            bool handleMessageToMe(const Message&);

            int _id;
            MessageHandler _messageHandler;
            MessageManager* _messageManagerPtr;
            std::vector<std::unique_ptr<Capsule>> _capsules;
        public:
            int _nextTimerId;
    };

}