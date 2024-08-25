#pragma once

#include "Capsule.h"

#include "CapsuleRunner.h"
#include <string>
#include <iostream>
#include "Message.h"
#include "SendMessage.h"
#include <chrono>
#include <stdexcept>
#include "RacePrinter.h"

class Main_Capsule: public mrt::Capsule{
    public:
        Main_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr, mrt::CapsuleRunner* timerRunnerPtr, int fps, int goal);
        int getId() override;
        void start() override;
        void handleMessage(const mrt::Message&) override;
        
        void connectRacer(int id, std::string name, std::string filename);
    
    private:
        void sendStartRaceSignal(int toId);
        void sendDistanceRequest(int toId);

        void handleTimeout(const mrt::TimeoutMessage&);

        void handleUpdateTimerTimeout(int timeouts);
        void handleDistanceResponse(const mrt::DistanceResponse&);
        void handleGoalReachedMessage(const mrt::GoalReached);

        int _id;
        std::vector<int> _racerIds;
        std::vector<std::string> _racerNames;
        std::vector<int> _racersXPos;
        mrt::CapsuleRunner* _capsuleRunnerPtr;
        mrt::CapsuleRunner* _timerRunnerPtr;
        int _updateTimerId;
        std::chrono::steady_clock::duration _updateTime;
        RacePrinter _racePrinter;
        int _responseCount;
        int _goal;
        int _winnerIndex;
        enum State{WaitForUpdate, GetPositionsDuringRace, GetPositionsAfterRace, End};
        State _state;
};