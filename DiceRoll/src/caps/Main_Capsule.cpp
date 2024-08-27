#include "Main_Capsule.h"

#include "CapsuleRunner.h"

#include <chrono>
#include <stdexcept>
#include <cassert>

Main_Capsule::Main_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr, mrt::CapsuleRunner* timerRunnerPtr, int fps)
{
    _id = id;
    _capsuleRunnerPtr = capsuleRunnerPtr;
    _timerRunnerPtr = timerRunnerPtr;
    _updateTime = std::chrono::milliseconds(1000/fps);
}

int Main_Capsule::getId(){
    return _id;
}

void Main_Capsule::connect(int diceId){
    _diceId = diceId;
}

mrt::RespondDiceValue Main_Capsule::invokeDiceValue(int toId){
    mrt::VoidMessage request = mrt::VoidMessage::RequestDiceValue;
    mrt::SendMessage sendMessage;
    sendMessage.toId = toId;
    sendMessage.message = request;
    mrt::Message receivedMessage = _capsuleRunnerPtr->invokeMessage(sendMessage);
    assert(std::holds_alternative<mrt::RespondDiceValue>(receivedMessage));
    return std::get<mrt::RespondDiceValue>(receivedMessage);
}

void Main_Capsule::receiveMessage(const mrt::Message& message){
    if(std::holds_alternative<mrt::TimeoutMessage>(message)){
        handleTimeout(std::get<mrt::TimeoutMessage>(message));
        return;
    }
    
    throw std::invalid_argument("Main_Capsule unable to handle message with index " + std::to_string(message.index()));
}

void Main_Capsule::handleTimeout(const mrt::TimeoutMessage& timeoutMessage){
    if(_updateTimerId == timeoutMessage.timerId){
        update(timeoutMessage.timeouts);
        return;
    }
    
    throw std::invalid_argument("Main_Capsule received unhandled timeoutMessage with id " + std::to_string(timeoutMessage.timerId));
}

void Main_Capsule::start(){
    _updateTimerId = _timerRunnerPtr->informEvery(_id, _updateTime);
    update(1);
    _state = State::Running;
}

void Main_Capsule::stop(){
    _capsuleRunnerPtr->stop();
    _timerRunnerPtr->cancelTimer(_updateTimerId);
    _state = State::End;
}

void Main_Capsule::update(int timeouts){
    if(_state != State::Running){
        return;
    }

    Gui::Event event = _gui.update();
    switch(event){
        case Gui::Event::End:
            stop();
            return;
        case Gui::Event::DiceRoll:
        {
            int diceRoll = invokeDiceValue(_diceId).value;
            _gui.setDiceValue(diceRoll);
        }
            return;
        default:
            return;
    }
}