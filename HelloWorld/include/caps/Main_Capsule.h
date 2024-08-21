#pragma once

#include "Capsule.h"

#include "CapsuleRunner.h"
#include "Message.h"
#include "SendMessage.h"
#include <chrono>
#include <stdexcept>
#include "Gui.h"

class Main_Capsule: public mrt::Capsule{
    public:
        Main_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr, mrt::CapsuleRunner* timerRunnerPtr, int fps);
        int getId();
        void start();
        void handleMessage(mrt::Message message);
    
    private:
        void stop();

        void handleTimeout(mrt::TimeoutMessage message);
        void update(int timeouts);

        int _id;
        mrt::CapsuleRunner* _capsuleRunnerPtr;
        mrt::CapsuleRunner* _timerRunnerPtr;
        int _updateTimerId;
        std::chrono::steady_clock::duration _updateTime;
        Gui _gui;
        enum State{Running, End};
        State _state;
};