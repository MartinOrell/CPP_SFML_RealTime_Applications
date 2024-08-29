#pragma once

#include "Capsule.h"

#include "Message.h"
#include "SendMessage.h"
#include <string>
#include "Gui.h"

namespace mrt{
    class CapsuleRunner;
}

class Main_Capsule: public mrt::Capsule{
    public:
        Main_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr);
        int getId() override;
        void start() override;
        void receiveMessage(const mrt::Message&) override;
        
        void connectUI(int uiId);
        void connectRacer(int id);
    
    private:
        void sendStartRaceSignal(int toId);
        void sendFinishRaceMessage(int toId, int winnerId);

        void handleClickMessage();
        void handleGoalReachedMessage(const mrt::GoalReached&);

        void startRace();
        void goalReached(const mrt::GoalReached&);

        int _id;
        int _uiId;
        std::vector<int> _racerIds;
        mrt::CapsuleRunner* _capsuleRunnerPtr;
        enum State{WaitForStartInput, Racing, End};
        State _state;
};