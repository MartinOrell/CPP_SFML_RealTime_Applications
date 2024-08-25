#include "CapsuleRunner.h"

#include "Capsule.h"
#include "MessageManager.h"

#include <stdexcept>
#include <string>
#include <cassert>

using namespace mrt;

CapsuleRunner::CapsuleRunner(int id, MessageManager* messageManagerPtr)
:   _id{id},
    _messageManagerPtr{messageManagerPtr},
    _nextTimerId{0}{
        if(_messageManagerPtr!=nullptr){
            _messageManagerPtr->addCapsuleRunnerPtr(this);
        }
    }

CapsuleRunner::CapsuleRunner(CapsuleRunner&& rhs)
: _id{std::move(rhs._id)}
, _messageHandler{std::move(rhs._messageHandler)}
, _messageManagerPtr{std::move(rhs._messageManagerPtr)}
, _nextTimerId{std::move(rhs._nextTimerId)}{
    if(_messageManagerPtr!=nullptr){
        _messageManagerPtr->replaceCapsuleRunnerPtr(this, &rhs);
    }
}

void CapsuleRunner::addCapsule(std::unique_ptr<Capsule> capsule){
    _capsules.push_back(std::move(capsule));
}

bool CapsuleRunner::isResponsible(int id){
    if(id == _id){
        return true;
    }
    for(int i = 0; i < _capsules.size();i++){
        if(id == _capsules.at(i)->getId()){
            return true;
        }
    }
    return false;
}

void CapsuleRunner::run(){
    
    for(int i = 0; i < _capsules.size();i++){
        _capsules.at(i)->start();
    }

    bool running = true;
    while(running){
        std::optional<SendMessage> optSendMessage = _messageHandler.receiveMessage();
        if(optSendMessage.has_value()){
            running = handleMessage(optSendMessage.value());
        }
    }
}

void CapsuleRunner::stop(){
    VoidMessage outMessage = VoidMessage::EndMessage;
    SendMessage sendMessage;
    sendMessage.toId = _id;
    sendMessage.message = outMessage;
    _messageHandler.sendMessage(sendMessage);
}

void CapsuleRunner::sendMessage(const SendMessage& message){
    if(isResponsible(message.toId)){
        _messageHandler.sendMessage(message);
        return;
    }

    if(_messageManagerPtr == nullptr){
        std::string errorMsg =
            "CapsuleRunner[" +
            std::to_string(_id) +
            "] is not responsible for Capsule[" +
            std::to_string(message.toId) +
            "] and has no messageManager to send a message to that capsule";
        throw std::logic_error(errorMsg);
    }

    _messageManagerPtr->sendMessage(message);
}

void CapsuleRunner::mergeOrSendMessage(const SendMessage& message){
    if(isResponsible(message.toId)){
        _messageHandler.mergeOrSendMessage(message);
        return;
    }

    if(_messageManagerPtr == nullptr){
        std::string errorMsg =
            "CapsuleRunner[" +
            std::to_string(_id) +
            "] is not responsible for Capsule[" +
            std::to_string(message.toId) +
            "] and has no messageManager to mergeOrSend a message to that capsule";
        throw std::logic_error(errorMsg);
    }
    
    _messageManagerPtr->mergeOrSendMessage(message);
}

Message CapsuleRunner::invokeMessage(const SendMessage& request){

    for(int i = 0; i < _capsules.size();i++){
        if(request.toId == _capsules.at(i)->getId()){
            return _capsules.at(i)->handleInvokeMessage(request.message);
        }
    }

    if(_messageManagerPtr == nullptr){
        std::string errorMsg =
            "CapsuleRunner[" +
            std::to_string(_id) +
            "] is not responsible for Capsule[" +
            std::to_string(request.toId) +
            "] and has no messageManager to invoke a message to that capsule";
        throw std::logic_error(errorMsg);
    }

    return _messageManagerPtr->invokeMessage(request);
}

int CapsuleRunner::informIn(int toId, std::chrono::steady_clock::duration duration){
    Timer timer;
    timer.id = _nextTimerId++;
    timer.toId = toId;
    timer.timeoutTime = std::chrono::steady_clock::now() + duration;
    timer.isRepeating = false;
    SendMessage message;
    message.toId = _id;
    message.message = timer;
    _messageHandler.sendMessage(message);
    return timer.id;
}

int CapsuleRunner::informEvery(int toId, std::chrono::steady_clock::duration interval){
    assert(interval > std::chrono::nanoseconds(0));
    Timer timer;
    timer.id = _nextTimerId++;
    timer.toId = toId;
    timer.timeoutTime = std::chrono::steady_clock::now() + interval;
    timer.isRepeating = true;
    timer.interval = interval;
    SendMessage message;
    message.toId = _id;
    message.message = timer;
    _messageHandler.sendMessage(message);
    return timer.id;
}

void CapsuleRunner::cancelTimer(int timerId){
    CancelTimer cancelTimer;
    cancelTimer.timerId = timerId;
    SendMessage message;
    message.toId = _id;
    message.message = cancelTimer;
    _messageHandler.sendMessage(message);
}

//handleMessage handles one message
//Returns false if the message is to stop running
//Returns true if successful
bool CapsuleRunner::handleMessage(const SendMessage& sendMessage){
    //check if message is to this capsuleRunner
    if(sendMessage.toId == _id){
        return handleMessageToMe(sendMessage.message);
    }

    //check if message is to a capsule owned by this capsuleRunner
    for(int i = 0; i < _capsules.size();i++){
        if(sendMessage.toId == _capsules.at(i)->getId()){
            _capsules.at(i)->handleMessage(sendMessage.message);
            return true;
        }
    }

    if(_messageManagerPtr == nullptr){
        std::string errorMsg =
            "CapsuleRunner[" +
            std::to_string(_id) +
            "] is not responsible for Capsule[" +
            std::to_string(sendMessage.toId) +
            "] and has no messageManager to send a message to that capsule";
        throw std::logic_error(errorMsg);
    }

    //Send the message to another capsuleRunner
    if(std::holds_alternative<TimeoutMessage>(sendMessage.message)){
        _messageManagerPtr->mergeOrSendMessage(sendMessage);
        return true;
    }
    _messageManagerPtr->sendMessage(sendMessage);
    return true;
}

//handleMessageToMe handles one message sent to this capsule
//Returns false if the message is to stop running
//Returns true otherwise
bool CapsuleRunner::handleMessageToMe(const Message& message){
    if(std::holds_alternative<VoidMessage>(message)){
        VoidMessage voidMessage = std::get<VoidMessage>(message);
        if(voidMessage == VoidMessage::EndMessage){
            return false;
        }
        else{
            std::string errorMsg =
                "CapsuleRunner[" +
                std::to_string(_id) + 
                "] can't handle Voidmessage: " +
                std::to_string(voidMessage);
            throw std::invalid_argument(errorMsg);
        }
    }
    else if(std::holds_alternative<Timer>(message)){
        Timer timer = std::get<Timer>(message);
        _messageHandler.addTimer(timer);
        return true;
    }
    else if (std::holds_alternative<CancelTimer>(message)){
        int timerId = std::get<CancelTimer>(message).timerId;
        _messageHandler.removeTimer(timerId);
        return true;
    }
    else{
        std::string errorMsg =
            "CapsuleRunner[" +
            std::to_string(_id) +
            "] can't handle message to him with type index: " +
            std::to_string(message.index());
        throw std::invalid_argument(errorMsg);
    }
}