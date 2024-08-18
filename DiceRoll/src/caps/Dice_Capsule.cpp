#include "Dice_Capsule.h"
#include "CapsuleRunner.h"

Dice_Capsule::Dice_Capsule(int id, CapsuleRunner* capsuleRunnerPtr, CapsuleRunner* timerRunnerPtr, int rollSpeed, int min, int max)
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

Message Dice_Capsule::handleInvokeMessage(Message message){
    if(std::holds_alternative<VoidMessage>(message)){
        VoidMessage voidMessage = std::get<VoidMessage>(message);
        if(voidMessage == VoidMessage::RequestDiceValue){
            return invokeDiceValueResponse();
        }
        throw std::invalid_argument("Dice_Capsule unable to invoke voidMessage with index " + std::to_string(voidMessage));
    }
    throw std::invalid_argument("Dice_Capsule unable to invoke message with index " + std::to_string(message.index()));
}

Message Dice_Capsule::invokeDiceValueResponse(){
    RespondDiceValue response;
    response.value = _value;
    return response;
}

void Dice_Capsule::handleMessage(Message message){
    if(std::holds_alternative<TimeoutMessage>(message)){
        handleTimeout(std::get<TimeoutMessage>(message));
        return;
    }
    if(std::holds_alternative<VoidMessage>(message)){
        VoidMessage voidMessage = std::get<VoidMessage>(message);
        if(voidMessage == VoidMessage::EndMessage){
            stop();
            return;
        }
        throw std::invalid_argument("Dice_Capsule unable to handle voidMessage with index " + std::to_string(voidMessage));
    }
    throw std::invalid_argument("Dice_Capsule unable to handle message with index " + std::to_string(message.index()));
}

void Dice_Capsule::handleTimeout(TimeoutMessage timeoutMessage){
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