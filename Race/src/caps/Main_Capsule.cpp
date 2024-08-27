#include "Main_Capsule.h"

Main_Capsule::Main_Capsule(int id, mrt::CapsuleRunner* capsuleRunnerPtr, mrt::CapsuleRunner* timerRunnerPtr, int fps, int goal)
: _racePrinter(goal)
{
    _id = id;
    _capsuleRunnerPtr = capsuleRunnerPtr;
    _timerRunnerPtr = timerRunnerPtr;
    _updateTime = std::chrono::milliseconds(1000/fps);
    _goal = goal;
}

int Main_Capsule::getId(){
    return _id;
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

void Main_Capsule::connectRacer(int id, std::string name, std::string filename){
    _racerIds.push_back(id);
    _racerNames.push_back(name);
    _racersXPos.push_back(0);
    _racePrinter.addRacer(id, filename);
}

void Main_Capsule::receiveMessage(const mrt::Message& message){
    if(std::holds_alternative<mrt::TimeoutMessage>(message)){
        handleTimeout(std::get<mrt::TimeoutMessage>(message));
        return;
    }
    else if(std::holds_alternative<mrt::DistanceResponse>(message)){
        handleDistanceResponse(std::get<mrt::DistanceResponse>(message));
        return;
    }
    else if(std::holds_alternative<mrt::GoalReached>(message)){
        handleGoalReachedMessage(std::get<mrt::GoalReached>(message));
        return;
    }
    
    throw std::invalid_argument("Main_Capsule unable to handle message with index " + std::to_string(message.index()));
}

void Main_Capsule::handleTimeout(const mrt::TimeoutMessage& timeoutMessage){
    if(_updateTimerId == timeoutMessage.timerId){
        handleUpdateTimerTimeout(timeoutMessage.timeouts);
    }
    else{
        throw std::invalid_argument("Main_Capsule[" + std::to_string(_id) + "] Received unhandled timeoutMessage with id " + std::to_string(timeoutMessage.timerId));
    }
}

void Main_Capsule::start(){
    _racePrinter.initPrint();
    std::cout << "The race has started!" << std::endl;
    for(int i = 0; i < _racerIds.size(); i++){
        sendStartRaceSignal(_racerIds.at(i));
    }
    _updateTimerId = _timerRunnerPtr->informEvery(_id, _updateTime);
    _racePrinter.print(_racersXPos);
    _state = State::WaitForUpdate;
}

void Main_Capsule::handleUpdateTimerTimeout(int timeouts){
    if(_state != State::WaitForUpdate){
        return;
    }

    _responseCount = 0;
    for(int i = 0; i < _racerIds.size(); i++){
        sendDistanceRequest(_racerIds.at(i));
    }
    _state = State::GetPositionsDuringRace;
}

void Main_Capsule::handleDistanceResponse(const mrt::DistanceResponse& message){
    switch(_state){
        case State::GetPositionsDuringRace:
        case State::GetPositionsAfterRace:
            break;
        default:
            throw std::runtime_error("Main_Capsule Received DistanceResponse in state " + std::to_string(_state));
    }

    _responseCount++;
    int index = -1;
    for(int i = 0; i < _racerIds.size(); i++){
        if(_racerIds.at(i) == message.fromId){
            index = i;
            break;
        }
    }
    if(index == -1){
        throw std::runtime_error("Main_Capsule received DistanceResponse from racer[" + std::to_string(message.fromId) + "], but unable to find that racer");
    }
    _racersXPos.at(index) = message.dist;

    switch(_state){
        case State::GetPositionsDuringRace:
            if(_responseCount >= _racerIds.size()){
                _racePrinter.print(_racersXPos);
                _state = State::WaitForUpdate;
            }
            return;
        case State::GetPositionsAfterRace:
            if(_responseCount >= _racerIds.size()-1){
                _racePrinter.print(_racersXPos);
                std::cout << _racerNames.at(_winnerIndex) << " won!" << std::endl;
                _capsuleRunnerPtr->stop();
                _state = State::End;
            }
            return;
    }
}

void Main_Capsule::handleGoalReachedMessage(mrt::GoalReached message){
    switch(_state){
        case State::WaitForUpdate:
        case State::GetPositionsDuringRace:
            break;
        default:
            return;
    }

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