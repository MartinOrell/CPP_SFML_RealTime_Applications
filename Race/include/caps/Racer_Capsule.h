#pragma once

#include "Capsule.h"

#include <string>
#include "CapsuleRunner.h"
#include "RacerProfile.h"
#include "SendMessage.h"

class Racer_Capsule: public mrt::Capsule{
    public:
        Racer_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr, mrt::CapsuleRunner* timerRunnerPtr, RacerProfile racerProfile, int goal);
        int getId() override;
        std::string getName();
        std::string getAsciiFilename();
        void receiveMessage(const mrt::Message&) override;
        void start() override;

        void connect(int mainId);
    private:
        void sendDistanceResponse(int toId);
        void sendGoalReached(int toId);

        void handleTimeout(const mrt::TimeoutMessage&);
        void handleStartSignal();
        void handleDistanceRequest();
        void handleWaitTimerTimeout(int timeouts);
        void handleStepTimerTimeout(int timeouts);

        int _id;
        int _mainId;
        mrt::CapsuleRunner* _capsuleRunnerPtr;
        mrt::CapsuleRunner* _timerRunnerPtr;
        RacerProfile _profile;
        int _waitTimerId;
        int _stepTimerId;
        std::chrono::steady_clock::duration _stepTime;
        int _numSteps;
        int _stamina;
        int _goal;
        enum State{WaitForStartSignal, ReactToStartSignal, Running, Resting};
        State _state;
};