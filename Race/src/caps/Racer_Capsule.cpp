#include "Racer_Capsule.h"

#include "CapsuleRunner.h"

#include <chrono>
#include <stdexcept>
#include <cassert>

Racer_Capsule::Racer_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr, mrt::CapsuleRunner* timerRunnerPtr, RacerProfile racerProfile, int goal)
: _id{id}
, _capsuleRunnerPtr{capsuleRunnerPtr}
, _timerRunnerPtr{timerRunnerPtr}
, _profile{racerProfile}
, _stepTime{std::chrono::nanoseconds(1000000000/racerProfile.speed)}
, _goal{goal}{}

int Racer_Capsule::getId(){
    return _id;
}

std::string Racer_Capsule::getName(){
    return _profile.name;
}

std::string Racer_Capsule::getArtFilename(){
    return _profile.artFilename;
}

void Racer_Capsule::connectMain(int mainId){
    _mainId = mainId;
}

void Racer_Capsule::connectUI(int uiId){
    _uiId = uiId;
}

void Racer_Capsule::sendDistanceResponse(int toId){
    mrt::DistanceResponse outMessage;
    outMessage.fromId = _id;
    outMessage.dist = _numSteps;
    mrt::SendMessage sendMessage;
    sendMessage.toId = toId;
    sendMessage.message = outMessage;
    _capsuleRunnerPtr->sendMessage(sendMessage);
}

void Racer_Capsule::sendGoalReached(int toId){
    mrt::GoalReached outMessage;
    outMessage.racerId = _id;
    mrt::SendMessage sendMessage;
    sendMessage.toId = toId;
    sendMessage.message = outMessage;
    _capsuleRunnerPtr->sendMessage(sendMessage);
}

void Racer_Capsule::receiveMessage(const mrt::Message& message){
    if(std::holds_alternative<mrt::TimeoutMessage>(message)){
        handleTimeout(std::get<mrt::TimeoutMessage>(message));
        return;
    }
    if(std::holds_alternative<mrt::VoidMessage>(message)){
        auto voidMessage = std::get<mrt::VoidMessage>(message);
        switch(voidMessage){
            case mrt::VoidMessage::StartSignal:
                handleStartSignal();
                return;
            case mrt::VoidMessage::StopSignal:
                handleStopSignal();
                return;
            case mrt::VoidMessage::DistanceRequest:
                handleDistanceRequest();
                return;
        }
        std::string errorMsg =
            "Racer_Capsule[" +
            std::to_string(_id) +
            "] unable to receive VoidMessage[" +
            std::to_string(voidMessage) +
            "]";
        throw std::invalid_argument(errorMsg);
    }
    std::string errorMsg =
        "Racer_Capsule[" +
        std::to_string(_id) +
        "] unable to receive Message[" +
        std::to_string(message.index()) +
        "]";
    throw std::invalid_argument(errorMsg);
}

void Racer_Capsule::handleTimeout(const mrt::TimeoutMessage& timeoutMessage){
    if(_waitTimerId == timeoutMessage.timerId){
        handleWaitTimerTimeout(timeoutMessage.timeouts);
    }
    else if(_stepTimerId == timeoutMessage.timerId){
        handleStepTimerTimeout(timeoutMessage.timeouts);
    }
}

void Racer_Capsule::handleStartSignal(){
    if(_state == State::WaitForStartSignal){
        hearStartSignal();
    }
}

void Racer_Capsule::handleStopSignal(){
    if(_state != State::WaitForStartSignal){
        stop();
    }
}

void Racer_Capsule::handleDistanceRequest(){
    sendDistanceResponse(_uiId);
}

void Racer_Capsule::handleWaitTimerTimeout(int timeouts){
    switch(_state){
        case State::ReactToStartSignal:
        case State::Resting:
            startRunning();
    }
}

void Racer_Capsule::handleStepTimerTimeout(int timeouts){
    if(_state == State::Running){
        step(timeouts);
    }
}

void Racer_Capsule::start(){
    _state = State::WaitForStartSignal;
}

void Racer_Capsule::hearStartSignal(){
    assert(_state == State::WaitForStartSignal);
    _numSteps = 0;
    _waitTimerId = _timerRunnerPtr->informIn(_id,_profile.reactionTime);
    _state = State::ReactToStartSignal;
}

void Racer_Capsule::startRunning(){
    assert(_state == State::ReactToStartSignal||_state == State::Resting);
    _stamina = _profile.stamina;
    _stepTimerId = _timerRunnerPtr->informEvery(_id, _stepTime);
    _state = State::Running;
}

void Racer_Capsule::step(int timeouts){
    assert(_state == State::Running);
    _numSteps++;
    _stamina--;
    if(_numSteps >= _goal){
        sendGoalReached(_mainId);
        _timerRunnerPtr->cancelTimer(_stepTimerId);
        _state = State::WaitForStartSignal;
    }
    else if(_stamina <= 0){
        _timerRunnerPtr->cancelTimer(_stepTimerId);
        _waitTimerId = _timerRunnerPtr->informIn(_id, _profile.restTime);
        _state = State::Resting;
    }
}

void Racer_Capsule::stop(){
    assert(_state != State::WaitForStartSignal);
    _timerRunnerPtr->cancelTimer(_stepTimerId);
    _state = State::WaitForStartSignal;
}