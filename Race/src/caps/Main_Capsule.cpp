#include "Main_Capsule.h"

#include "CapsuleRunner.h"

#include <chrono>
#include <stdexcept>
#include <cassert>

Main_Capsule::Main_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr)
: _id{id}
, _capsuleRunnerPtr{capsuleRunnerPtr}{}

int Main_Capsule::getId(){
    return _id;
}

void Main_Capsule::connectUI(int uiId){
    _uiId = uiId;
}

void Main_Capsule::connectRacer(int id){
    _racerIds.push_back(id);
}

void Main_Capsule::sendStartRaceSignal(int toId){
    mrt::VoidMessage outMessage = mrt::VoidMessage::StartSignal;
    mrt::SendMessage sendMessage;
    sendMessage.toId = toId;
    sendMessage.message = outMessage;
    _capsuleRunnerPtr->sendMessage(sendMessage);
}

void Main_Capsule::sendFinishRaceMessage(int toId, int winnerId){
    mrt::GoalReached outMessage;
    outMessage.racerId = winnerId;
    mrt::SendMessage sendMessage;
    sendMessage.toId = toId;
    sendMessage.message = outMessage;
    _capsuleRunnerPtr->sendMessage(sendMessage);
}

void Main_Capsule::sendStopSignal(int toId){
    mrt::SendMessage sendMessage;
    sendMessage.toId = toId;
    sendMessage.message = mrt::VoidMessage::StopSignal;
    _capsuleRunnerPtr->sendMessage(sendMessage);
}

void Main_Capsule::receiveMessage(const mrt::Message& message){
    if(std::holds_alternative<mrt::VoidMessage>(message)){
        auto voidMessage = std::get<mrt::VoidMessage>(message);
        if(voidMessage == mrt::VoidMessage::ClickMessage){
            handleClickMessage();
            return;
        }
        std::string errorMsg =
            "Main_Capsule[" +
            std::to_string(_id) +
            "] unable to receive VoidMessage[" +
            std::to_string(voidMessage) +
            "]";
        throw std::invalid_argument(errorMsg);
    }
    if(std::holds_alternative<mrt::GoalReached>(message)){
        handleGoalReachedMessage(std::get<mrt::GoalReached>(message));
        return;
    }
    
    std::string errorMsg =
        "Main_Capsule[" +
        std::to_string(_id) +
        "] unable to receive Message[" +
        std::to_string(message.index()) +
        "]";
    throw std::invalid_argument(errorMsg);
}

void Main_Capsule::handleGoalReachedMessage(const mrt::GoalReached& message){
    if(_state == State::Racing){
        goalReached(message);
    }
}

void Main_Capsule::handleClickMessage(){
    if(_state == State::WaitForStartInput){
        startRace();
    }
}

void Main_Capsule::start(){
    _state = State::WaitForStartInput;
}

void Main_Capsule::startRace(){
    assert(_state == State::WaitForStartInput);
    sendStartRaceSignal(_uiId);
    for(int i = 0; i < _racerIds.size(); i++){
        sendStartRaceSignal(_racerIds.at(i));
    }
    _state = State::Racing;
}

void Main_Capsule::goalReached(const mrt::GoalReached& message){
    assert(_state == State::Racing);
    sendFinishRaceMessage(_uiId, message.racerId);
    for(int i = 0; i < _racerIds.size(); i++){
        if(i != message.racerId){
            sendStopSignal(_racerIds.at(i));
        }
    }
    _state = State::End;
}