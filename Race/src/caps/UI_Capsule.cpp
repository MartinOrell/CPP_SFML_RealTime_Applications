#include "UI_Capsule.h"

#include "CapsuleRunner.h"

#include <chrono>
#include <cassert>

UI_Capsule::UI_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr, mrt::CapsuleRunner* timerRunnerPtr, int fps, int goal)
: _id{id}
, _capsuleRunnerPtr{capsuleRunnerPtr}
, _timerRunnerPtr{timerRunnerPtr}
, _updateTime{std::chrono::milliseconds(1000/fps)}
, _goal{goal}
, _ui{goal}{}

int UI_Capsule::getId(){
    return _id;
}

void UI_Capsule::connectMain(int mainId){
    _mainId = mainId;
}

void UI_Capsule::connectRacer(int id, std::string name, std::string filename){
    _racerIds.push_back(id);
    _racerNames.push_back(name);
    _racersXPos.push_back(0);
    _ui.addRacer(id, filename);
}

void UI_Capsule::setRacerPosition(const mrt::DistanceResponse& message){
    for(int i = 0; i < _racerIds.size(); i++){
        if(_racerIds.at(i) == message.fromId){
            _racersXPos.at(i) = message.dist;
            return;
        }
    }
    
    std::string errorMsg =
        "Main_Capsule[" +
        std::to_string(_id) +
        "] failed to set position for Racer[" +
        std::to_string(message.fromId) +
        "] because it can't find it";
    throw std::runtime_error(errorMsg);
}

void UI_Capsule::sendDistanceRequest(int toId){
    mrt::VoidMessage outMessage = mrt::VoidMessage::DistanceRequest;
    mrt::SendMessage sendMessage;
    sendMessage.toId = toId;
    sendMessage.message = outMessage;
    _capsuleRunnerPtr->sendMessage(sendMessage);
}

void UI_Capsule::sendClickMessage(int toId){
    mrt::VoidMessage outMessage = mrt::VoidMessage::ClickMessage;
    mrt::SendMessage sendMessage;
    sendMessage.toId = toId;
    sendMessage.message = outMessage;
    _capsuleRunnerPtr->sendMessage(sendMessage);
}

void UI_Capsule::receiveMessage(const mrt::Message& message){
    if(std::holds_alternative<mrt::TimeoutMessage>(message)){
        handleTimeout(std::get<mrt::TimeoutMessage>(message));
        return;
    }
    if(std::holds_alternative<mrt::DistanceResponse>(message)){
        handleDistanceResponse(std::get<mrt::DistanceResponse>(message));
        return;
    }
    if(std::holds_alternative<mrt::VoidMessage>(message)){
        auto voidMessage = std::get<mrt::VoidMessage>(message);
        if(voidMessage == mrt::VoidMessage::StartSignal){
            handleStartSignal();
        }
        return;
    }
    if(std::holds_alternative<mrt::GoalReached>(message)){
        handleFinishRaceMessage(std::get<mrt::GoalReached>(message));
    }
}

void UI_Capsule::handleTimeout(const mrt::TimeoutMessage& timeoutMessage){
    if(timeoutMessage.timerId == _updateTimerId){
        if(_state == State::WaitForUpdate){
            update();
        }
    }
}

void UI_Capsule::handleDistanceResponse(const mrt::DistanceResponse& message){
    if(_state == State::GetPositions){
        updateRacerPosition(message);
    }
}

void UI_Capsule::handleStartSignal(){
    if(_state != State::Closed){
        startRace();
    }
}

void UI_Capsule::handleFinishRaceMessage(const mrt::GoalReached& message){
    if(_state != State::Closed){
        finishRace(message.racerId);
    }
}

void UI_Capsule::start(){
    _ui.initPrint();
    _ui.setText("Click anywhere to start race");
    _updateTimerId = _timerRunnerPtr->informEvery(_id, _updateTime);
    _state = State::WaitForUpdate;
}

void UI_Capsule::startRace(){
    _ui.setText("The race has started!");
}

void UI_Capsule::finishRace(int winnerId){
    for(int i = 0; i < _racerIds.size(); i++){
        if(_racerIds.at(i) == winnerId){
            _ui.setText(_racerNames.at(i) + " won!");
            return;
        }
    }
}

void UI_Capsule::update(){
    assert(_state == State::WaitForUpdate);
    _responseCount = 0;
    for(int i = 0; i < _racerIds.size(); i++){
        sendDistanceRequest(_racerIds.at(i));
    }
    _state = State::GetPositions;
}

void UI_Capsule::updateRacerPosition(const mrt::DistanceResponse& message){
    assert(_state == State::GetPositions);
    _responseCount++;
    setRacerPosition(message);
    if(_responseCount >= _racerIds.size()){
        gui::Event event = _ui.update(_racersXPos);
        switch(event){
            case gui::Event::Exit:
                _capsuleRunnerPtr->stop();
                _state = State::Closed;
                return;
            case gui::Event::Click:
                sendClickMessage(_mainId);
                break;
        }
        _state = State::WaitForUpdate;
    }
}