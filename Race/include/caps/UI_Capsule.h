#pragma once

#include "Capsule.h"

#include "Message.h"
#include "SendMessage.h"
#include "Gui.h"

namespace mrt{
    class CapsuleRunner;
}

class UI_Capsule: public mrt::Capsule{
    public:
        UI_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr, mrt::CapsuleRunner* timerRunnerPtr, int fps, int goal);
        int getId() override;
        void start() override;
        void receiveMessage(const mrt::Message&) override;

        void connectMain(int mainId);
        void connectRacer(int id, std::string name, std::string filename);
    private:
        void sendDistanceRequest(int toId);
        void sendClickMessage(int toId);

        void handleTimeout(const mrt::TimeoutMessage&);
        void handleDistanceResponse(const mrt::DistanceResponse&);
        void handleStartSignal();
        void handleFinishRaceMessage(const mrt::GoalReached&);

        void setRacerPosition(const mrt::DistanceResponse& message);

        void updateRacerPosition(const mrt::DistanceResponse& message);

        void update();
        void startRace();
        void finishRace(int winnerId);

        int _id;
        int _mainId;
        std::vector<int> _racerIds;
        mrt::CapsuleRunner* _capsuleRunnerPtr;
        mrt::CapsuleRunner* _timerRunnerPtr;
        int _updateTimerId;
        std::chrono::steady_clock::duration _updateTime;
        gui::UI _ui;
        enum State{WaitForUpdate, GetPositions, Closed};
        State _state;

        int _goal;
        std::vector<std::string> _racerNames;
        std::vector<int> _racersXPos;
        int _responseCount;
};