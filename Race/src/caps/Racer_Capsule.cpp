#include "Racer_Capsule.h"

Racer_Capsule::Racer_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr, mrt::CapsuleRunner* timerRunnerPtr, RacerProfile racerProfile, int goal){
    _id = id;
    _capsuleRunnerPtr = capsuleRunnerPtr;
    _timerRunnerPtr = timerRunnerPtr;
    _profile = racerProfile;
    _stepTime = std::chrono::nanoseconds(1000000000/racerProfile.speed);
    _numSteps = 0;
    _stamina = racerProfile.stamina;
    _goal = goal;
}

int Racer_Capsule::getId(){
    return _id;
}

std::string Racer_Capsule::getName(){
    return _profile.name;
}

std::string Racer_Capsule::getAsciiFilename(){
    return _profile.asciiFilename;
}

void Racer_Capsule::handleMessage(mrt::Message message){
    if(std::holds_alternative<mrt::TimeoutMessage>(message)){
        handleTimeout(std::get<mrt::TimeoutMessage>(message));
        return;
    }
    else if(std::holds_alternative<mrt::VoidMessage>(message)){
        switch(std::get<mrt::VoidMessage>(message)){
            case mrt::VoidMessage::StartSignal:
                handleStartSignal();
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

void Racer_Capsule::connect(int mainId){
    _mainId = mainId;
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
    outMessage.fromId = _id;
    mrt::SendMessage sendMessage;
    sendMessage.toId = toId;
    sendMessage.message = outMessage;
    _capsuleRunnerPtr->sendMessage(sendMessage);
}

void Racer_Capsule::start(){
    _state = State::WaitForStartSignal;
}

void Racer_Capsule::handleTimeout(mrt::TimeoutMessage timeoutMessage){
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
    _waitTimerId = _timerRunnerPtr->informIn(_id,_profile.reactionTime);
    _state = State::ReactToStartSignal;
}

void Racer_Capsule::handleDistanceRequest(){
    sendDistanceResponse(_mainId);
}

void Racer_Capsule::handleWaitTimerTimeout(int timeouts){
    switch(_state){
        case State::ReactToStartSignal:
            _stepTimerId = _timerRunnerPtr->informEvery(_id, _stepTime);
            _state = Running;
            return;
        case State::Resting:
            _stamina = _profile.stamina;
            _stepTimerId = _timerRunnerPtr->informEvery(_id, _stepTime);
            _state = Running;
            return;
    }
    throw std::runtime_error("Racer_Capsule[" + std::to_string(_id) + "] Received WaitTimerTimeout in state " + std::to_string(_state));
}

void Racer_Capsule::handleStepTimerTimeout(int timeouts){
    switch(_state){
        case State::Running:
            break;
        default:
            return;
    }
    _numSteps++;
    _stamina--;
    if(_numSteps >= _goal){
        sendGoalReached(_mainId);
        _timerRunnerPtr->cancelTimer(_stepTimerId);
        _state = State::WaitForStartSignal;
        return;
    }
    if(_stamina <= 0){
        _timerRunnerPtr->cancelTimer(_stepTimerId);
        _waitTimerId = _timerRunnerPtr->informIn(_id, _profile.restTime);
        _state = State::Resting;
        return;
    }
}