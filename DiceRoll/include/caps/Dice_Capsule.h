#pragma once

#include "Capsule.h"

#include <string>
#include <iostream>
#include "Message.h"
#include "SendMessage.h"
#include <chrono>
#include <stdexcept>

class CapsuleRunner;

class Dice_Capsule: public Capsule{
    public:
        Dice_Capsule(int id, CapsuleRunner* capsuleRunnerPtr, CapsuleRunner* timerRunnerPtr, int rollSpeed, int min, int max);
        int getId();
        void start();
        void stop();
        void handleMessage(Message message);
        Message handleInvokeMessage(Message message);
    
    private:

        void handleTimeout(TimeoutMessage message);
        void update(int timeouts);
        Message invokeDiceValueResponse();

        int _id;
        CapsuleRunner* _capsuleRunnerPtr;
        CapsuleRunner* _timerRunnerPtr;
        int _updateTimerId;
        std::chrono::steady_clock::duration _updateTime;
        int _value;
        int _minValue;
        int _maxValue;
        enum State{Running, End};
        State _state;
};