#include "Main_Capsule.h"

#include "CapsuleRunner.h"

#include <chrono>
#include <stdexcept>
#include <cassert>

Main_Capsule::Main_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr, mrt::CapsuleRunner* timerRunnerPtr, int fps, int goal)
: _id{id}
, _capsuleRunnerPtr{capsuleRunnerPtr}
, _timerRunnerPtr{timerRunnerPtr}
, _updateTime{std::chrono::milliseconds(1000/fps)}
, _goal{goal}
, _ui{goal}{}

int Main_Capsule::getId(){
    return _id;
}

void Main_Capsule::connectRacer(int id, std::string name, std::string filename){
    _racerIds.push_back(id);
    _racerNames.push_back(name);
    _racersXPos.push_back(0);
    _ui.addRacer(id, filename);
}

void Main_Capsule::updateRacerPosition(const mrt::DistanceResponse& message){
    for(int i = 0; i < _racerIds.size(); i++){
        if(_racerIds.at(i) == message.fromId){
            _racersXPos.at(i) = message.dist;
            return;
        }
    }
    
    std::string errorMsg =
        "Main_Capsule[" +
        std::to_string(_id) +
        "] failed to update position for Racer[" +
        std::to_string(message.fromId) +
        "] because it can't find it";
    throw std::runtime_error(errorMsg);
}

void Main_Capsule::sendStartRaceSignal(int toId){
    mrt::VoidMessage outMessage = mrt::VoidMessage::StartSignal;
    mrt::SendMessage sendMessage;
    sendMessage.toId = toId;
    sendMessage.message = outMessage;
    _capsuleRunnerPtr->sendMessage(sendMessage);
}

void Main_Capsule::sendDistanceRequest(int toId){
    mrt::VoidMessage outMessage = mrt::VoidMessage::DistanceRequest;
    mrt::SendMessage sendMessage;
    sendMessage.toId = toId;
    sendMessage.message = outMessage;
    _capsuleRunnerPtr->sendMessage(sendMessage);
}

void Main_Capsule::receiveMessage(const mrt::Message& message){
    if(std::holds_alternative<mrt::TimeoutMessage>(message)){
        handleTimeout(std::get<mrt::TimeoutMessage>(message));
        return;
    }
    if(std::holds_alternative<mrt::DistanceResponse>(message)){
        handleDistanceResponse(std::get<mrt::DistanceResponse>(message));
        return;
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

void Main_Capsule::handleTimeout(const mrt::TimeoutMessage& timeoutMessage){
    if(timeoutMessage.timerId == _updateTimerId){
        switch(_state){
            case State::WaitForStartInput:
                updateBeforeRaceStart(timeoutMessage.timeouts);
                return;
            case State::WaitForUpdate:
                updateDuringRace(timeoutMessage.timeouts);
        }
    }
}

void Main_Capsule::handleDistanceResponse(const mrt::DistanceResponse& message){
    switch(_state){
        case State::GetPositionsBeforeRace:
            updateRacerPositionBeforeRace(message);
            return;
        case State::GetPositionsDuringRace:
            updateRacerPositionDuringRace(message);
            return;
        case State::GetPositionsAfterRace:
            updateRacerPositionAfterRace(message);
            return;
    }
    std::string errorMsg =
        "Main_Capsule[" +
        std::to_string(_id) +
        "] received DistanceResponse in State[" +
        std::to_string(_state) +
        "]";
    throw std::runtime_error(errorMsg);
}

void Main_Capsule::handleGoalReachedMessage(const mrt::GoalReached& message){
    switch(_state){
        case State::WaitForUpdate:
        case State::GetPositionsDuringRace:
            goalReached(message);
    }
}

void Main_Capsule::start(){
    _ui.initPrint();
    _ui.setText("Click anywhere to start race");
    _updateTimerId = _timerRunnerPtr->informEvery(_id, _updateTime);
    _state = State::WaitForStartInput;
}

void Main_Capsule::startRace(){
    _ui.setText("The race has started!");
    for(int i = 0; i < _racerIds.size(); i++){
        sendStartRaceSignal(_racerIds.at(i));
    }
    gui::Event event = _ui.update(_racersXPos);
    if(event == gui::Event::Exit){
        _capsuleRunnerPtr->stop();
        _state = State::End;
        return;
    }
    _state = State::WaitForUpdate;
}

void Main_Capsule::updateBeforeRaceStart(int timeouts){
    assert(_state == State::WaitForStartInput);
    _responseCount = 0;
    for(int i = 0; i < _racerIds.size(); i++){
        sendDistanceRequest(_racerIds.at(i));
    }
    _state = State::GetPositionsBeforeRace;
}

void Main_Capsule::updateDuringRace(int timeouts){
    assert(_state == State::WaitForUpdate||_state);
    _responseCount = 0;
    for(int i = 0; i < _racerIds.size(); i++){
        sendDistanceRequest(_racerIds.at(i));
    }
    _state = State::GetPositionsDuringRace;
}

void Main_Capsule::updateRacerPositionBeforeRace(const mrt::DistanceResponse& message){
    assert(_state == State::GetPositionsBeforeRace);
    _responseCount++;
    updateRacerPosition(message);
    if(_responseCount >= _racerIds.size()){
        gui::Event event = _ui.update(_racersXPos);
        if(event == gui::Event::Exit){
            _capsuleRunnerPtr->stop();
            _state = State::End;
            return;
        }
        if(event == gui::Event::StartRace){
            startRace();
            return;
        }
        _state = State::WaitForStartInput;
    }
}

void Main_Capsule::updateRacerPositionDuringRace(const mrt::DistanceResponse& message){
    assert(_state == State::GetPositionsDuringRace);
    _responseCount++;
    updateRacerPosition(message);
    if(_responseCount >= _racerIds.size()){
        gui::Event event = _ui.update(_racersXPos);
        if(event == gui::Event::Exit){
            _capsuleRunnerPtr->stop();
            _state = State::End;
            return;
        }
        _state = State::WaitForUpdate;
    }
}

void Main_Capsule::updateRacerPositionAfterRace(const mrt::DistanceResponse& message){
    assert(_state == State::GetPositionsAfterRace);
    _responseCount++;
    updateRacerPosition(message);
    if(_responseCount >= _racerIds.size()-1){
        gui::Event event = _ui.update(_racersXPos);
        _ui.setText(_racerNames.at(_winnerIndex) + " won!");
        _capsuleRunnerPtr->stop();
        _state = State::End;
    }
}

void Main_Capsule::goalReached(const mrt::GoalReached& message){
    assert(_state == State::WaitForUpdate||_state == State::GetPositionsDuringRace);
    _timerRunnerPtr->cancelTimer(_updateTimerId);

    _responseCount = 0;
    for(int i = 0; i < _racerIds.size(); i++){
        if(_racerIds.at(i) == message.fromId){
            _winnerIndex = i;
            _racersXPos.at(i) = _goal;
        }
        else{
            sendDistanceRequest(_racerIds.at(i));
        }
    }
    _state = State::GetPositionsAfterRace;
}