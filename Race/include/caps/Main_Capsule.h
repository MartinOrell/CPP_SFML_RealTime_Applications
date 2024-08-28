#pragma once

#include "Capsule.h"

#include "Message.h"
#include "SendMessage.h"
#include <string>
#include "GUI.h"

namespace mrt{
    class CapsuleRunner;
}

class Main_Capsule: public mrt::Capsule{
    public:
        Main_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr, mrt::CapsuleRunner* timerRunnerPtr, int fps, int goal);
        int getId() override;
        void start() override;
        void receiveMessage(const mrt::Message&) override;
        
        void connectRacer(int id, std::string name, std::string filename);
    
    private:
        void sendStartRaceSignal(int toId);
        void sendDistanceRequest(int toId);

        void handleTimeout(const mrt::TimeoutMessage&);
        void handleDistanceResponse(const mrt::DistanceResponse&);
        void handleGoalReachedMessage(const mrt::GoalReached&);

        void updateRacerPosition(const mrt::DistanceResponse& message);

        void update(int timeouts);
        void updateRacerPositionDuringRace(const mrt::DistanceResponse& message);
        void updateRacerPositionAfterRace(const mrt::DistanceResponse& message);
        void goalReached(const mrt::GoalReached&);

        int _id;
        std::vector<int> _racerIds;
        std::vector<std::string> _racerNames;
        std::vector<int> _racersXPos;
        mrt::CapsuleRunner* _capsuleRunnerPtr;
        mrt::CapsuleRunner* _timerRunnerPtr;
        int _updateTimerId;
        std::chrono::steady_clock::duration _updateTime;
        gui::UI _ui;
        int _responseCount;
        int _goal;
        int _winnerIndex;
        enum State{WaitForUpdate, GetPositionsDuringRace, GetPositionsAfterRace, End};
        State _state;
};