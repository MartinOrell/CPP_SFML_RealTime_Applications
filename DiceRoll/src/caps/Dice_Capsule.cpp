#include "Dice_Capsule.h"

#include "CapsuleRunner.h"

#include <chrono>
#include <stdexcept>

Dice_Capsule::Dice_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr, mrt::CapsuleRunner* timerRunnerPtr, int rollSpeed, int min, int max)
{
    _id = id;
    _capsuleRunnerPtr = capsuleRunnerPtr;
    _timerRunnerPtr = timerRunnerPtr;
    _updateTime = std::chrono::nanoseconds(1000000000/rollSpeed);

    _minValue = min;
    _maxValue = max;
}

int Dice_Capsule::getId(){
    return _id;
}

mrt::Message Dice_Capsule::handleInvokeMessage(const mrt::Message& message){
    if(std::holds_alternative<mrt::VoidMessage>(message)){
        mrt::VoidMessage voidMessage = std::get<mrt::VoidMessage>(message);
        if(voidMessage == mrt::VoidMessage::RequestDiceValue){
            return invokeDiceValueResponse();
        }
        throw std::invalid_argument("Dice_Capsule unable to invoke voidMessage with index " + std::to_string(voidMessage));
    }
    throw std::invalid_argument("Dice_Capsule unable to invoke message with index " + std::to_string(message.index()));
}

mrt::Message Dice_Capsule::invokeDiceValueResponse(){
    mrt::RespondDiceValue response;
    response.value = _value;
    return response;
}

void Dice_Capsule::handleMessage(const mrt::Message& message){
    if(std::holds_alternative<mrt::TimeoutMessage>(message)){
        handleTimeout(std::get<mrt::TimeoutMessage>(message));
        return;
    }
    if(std::holds_alternative<mrt::VoidMessage>(message)){
        mrt::VoidMessage voidMessage = std::get<mrt::VoidMessage>(message);
        if(voidMessage == mrt::VoidMessage::EndMessage){
            stop();
            return;
        }
        throw std::invalid_argument("Dice_Capsule unable to handle voidMessage with index " + std::to_string(voidMessage));
    }
    throw std::invalid_argument("Dice_Capsule unable to handle message with index " + std::to_string(message.index()));
}

void Dice_Capsule::handleTimeout(const mrt::TimeoutMessage& timeoutMessage){
    if(_updateTimerId == timeoutMessage.timerId){
        update(timeoutMessage.timeouts);
        return;
    }
    
    throw std::invalid_argument("Main_Capsule received unhandled timeoutMessage with id " + std::to_string(timeoutMessage.timerId));
}

void Dice_Capsule::start(){
    _updateTimerId = _timerRunnerPtr->informEvery(_id, _updateTime);
    _value = _minValue;
    _state = State::Running;
}

void Dice_Capsule::stop(){
    _timerRunnerPtr->cancelTimer(_updateTimerId);
    _state = State::End;
}

void Dice_Capsule::update(int timeouts){
    if(_state != State::Running){
        return;
    }
    _value+=timeouts;
    if(_value > _maxValue){
        _value = _minValue;
    }
}