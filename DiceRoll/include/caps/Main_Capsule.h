#pragma once

#include "Capsule.h"

#include "Message.h"
#include "SendMessage.h"
#include "Gui.h"

namespace mrt{
    class CapsuleRunner;
}

class Main_Capsule: public mrt::Capsule{
    public:
        Main_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr, mrt::CapsuleRunner* timerRunnerPtr, int fps);
        int getId() override;
        void start() override;
        void stop();
        void handleMessage(const mrt::Message&) override;
    
        void connect(int diceId);
    private:
        mrt::RespondDiceValue invokeDiceValue(int toId);

        void handleTimeout(const mrt::TimeoutMessage&);
        void update(int timeouts);

        int _id;
        int _diceId;
        mrt::CapsuleRunner* _capsuleRunnerPtr;
        mrt::CapsuleRunner* _timerRunnerPtr;
        int _updateTimerId;
        std::chrono::steady_clock::duration _updateTime;
        Gui _gui;
        enum State{Running, End};
        State _state;
};