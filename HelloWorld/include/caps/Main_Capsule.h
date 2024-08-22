#pragma once

#include "Capsule.h"

#include "SendMessage.h"
#include "Message.h"
#include "Gui.h"

namespace mrt{
    class CapsuleRunner;
}

class Main_Capsule: public mrt::Capsule{
    public:
        Main_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr, mrt::CapsuleRunner* timerRunnerPtr, int fps);
        int getId();
        void start();
        void handleMessage(const mrt::Message& message);
    
    private:
        void stop();

        void handleTimeout(const mrt::TimeoutMessage& message);
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