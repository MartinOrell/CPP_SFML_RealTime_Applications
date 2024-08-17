#include "Main_Capsule.h"
#include "CapsuleRunner.h"

Main_Capsule::Main_Capsule(int id, CapsuleRunner* capsuleRunnerPtr, CapsuleRunner* timerRunnerPtr, int fps)
{
    _id = id;
    _capsuleRunnerPtr = capsuleRunnerPtr;
    _timerRunnerPtr = timerRunnerPtr;
    _updateTime = std::chrono::milliseconds(1000/fps);
}

int Main_Capsule::getId(){
    return _id;
}

void Main_Capsule::handleMessage(Message message){
    if(std::holds_alternative<TimeoutMessage>(message)){
        handleTimeout(std::get<TimeoutMessage>(message));
        return;
    }
    
    throw std::invalid_argument("Main_Capsule unable to handle message with index " + std::to_string(message.index()));
}

void Main_Capsule::handleTimeout(TimeoutMessage timeoutMessage){
    if(_updateTimerId == timeoutMessage.timerId){
        update(timeoutMessage.timeouts);
        return;
    }
    
    throw std::invalid_argument("Main_Capsule received unhandled timeoutMessage with id " + std::to_string(timeoutMessage.timerId));
}

void Main_Capsule::start(){
    _updateTimerId = _timerRunnerPtr->informEvery(_id, _updateTime);
    if(!_gui.update()){
        _capsuleRunnerPtr->stop();
        _timerRunnerPtr->cancelTimer(_updateTimerId);
        _state = State::End;
        return;
    }
    _state = State::Running;
}

void Main_Capsule::update(int timeouts){
    if(_state != State::Running){
        return;
    }

    if(!_gui.update()){
        _capsuleRunnerPtr->stop();
        _timerRunnerPtr->cancelTimer(_updateTimerId);
        _state = State::End;
        return;
    }
    _state = State::Running;
}