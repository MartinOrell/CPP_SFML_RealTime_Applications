#pragma once

#include "Capsule.h"

#include "Message.h"
#include "SendMessage.h"
#include <chrono>
#include <stdexcept>

namespace mrt{
    class CapsuleRunner;
}

class Dice_Capsule: public mrt::Capsule{
    public:
        Dice_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr, mrt::CapsuleRunner* timerRunnerPtr, int rollSpeed, int min, int max);
        int getId();
        void start();
        void stop();
        void handleMessage(mrt::Message message);
        mrt::Message handleInvokeMessage(mrt::Message message);
    
    private:

        void handleTimeout(mrt::TimeoutMessage message);
        void update(int timeouts);
        mrt::Message invokeDiceValueResponse();

        int _id;
        mrt::CapsuleRunner* _capsuleRunnerPtr;
        mrt::CapsuleRunner* _timerRunnerPtr;
        int _updateTimerId;
        std::chrono::steady_clock::duration _updateTime;
        int _value;
        int _minValue;
        int _maxValue;
        enum State{Running, End};
        State _state;
};