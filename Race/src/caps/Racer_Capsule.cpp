#include "Racer_Capsule.h"

#include "CapsuleRunner.h"

#include <chrono>
#include <stdexcept>
#include <cassert>

Racer_Capsule::Racer_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr, mrt::CapsuleRunner* timerRunnerPtr, RacerProfile racerProfile, int goal){
    _id = id;
    _capsuleRunnerPtr = capsuleRunnerPtr;
    _timerRunnerPtr = timerRunnerPtr;
    _profile = racerProfile;
    _stepTime = std::chrono::nanoseconds(1000000000/racerProfile.speed);
    _numSteps = 0;
    _goal = goal;
}

int Racer_Capsule::getId(){
    return _id;
}

std::string Racer_Capsule::getName(){
    return _profile.name;
}

std::string Racer_Capsule::getArtFilename(){
    return _profile.artFilename;
}

void Racer_Capsule::receiveMessage(const mrt::Message& message){
    if(std::holds_alternative<mrt::TimeoutMessage>(message)){
        handleTimeout(std::get<mrt::TimeoutMessage>(message));
        return;
    }
    else if(std::holds_alternative<mrt::VoidMessage>(message)){
        switch(std::get<mrt::VoidMessage>(message)){
            case mrt::VoidMessage::StartSignal:
                handleStartSignal();
                return;
            case mrt::VoidMessage::StopSignal:
                handleStopSignal();
                return;
            case mrt::VoidMessage::DistanceRequest:
                handleDistanceRequest();
                return;
            default:
                throw std::invalid_argument("Racer_Capsule[" + std::to_string(_id) + "] unable to handle voidMessage with id " + std::to_string(std::get<mrt::VoidMessage>(message)));
        }
    }
    throw std::invalid_argument("Racer_Capsule[" + std::to_string(_id) + "] unable to handle message with index " + std::to_string(message.index()));
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

void Racer_Capsule::handleTimeout(const mrt::TimeoutMessage& timeoutMessage){
    if(_waitTimerId == timeoutMessage.timerId){
        handleWaitTimerTimeout(timeoutMessage.timeouts);
    }
    else if(_stepTimerId == timeoutMessage.timerId){
        handleStepTimerTimeout(timeoutMessage.timeouts);
    }
}

void Racer_Capsule::handleStartSignal(){
    if(_state != State::WaitForStartSignal){
        throw std::runtime_error("Racer_Capsule[" + std::to_string(_id) + "] Received StartSignal in state " + std::to_string(_state));
    }
    hearStartSignal();
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
            return;
    }
    std::string errorMsg =
        "Racer_Capsule[" +
        std::to_string(_id) +
        "] Received WaitTimerTimeout in State[" +
        std::to_string(_state) +
        "]";
    throw std::runtime_error(errorMsg);
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