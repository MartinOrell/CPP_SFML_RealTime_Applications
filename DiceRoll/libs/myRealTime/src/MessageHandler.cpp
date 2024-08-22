#include "MessageHandler.h"

#include "SendMessage.h"
#include "Timer.h"

#include <algorithm>

using namespace mrt;

MessageHandler::MessageHandler(){
    _waitForMessageMutex.lock();
}

MessageHandler::MessageHandler(MessageHandler&& rhs)
: _messageStack(std::move(rhs._messageStack))
, _timers(std::move(rhs._timers)){
    //note: Since mutexes are unmovable they are not copied
    _waitForMessageMutex.lock();
}

void MessageHandler::addTimer(const Timer& timer){
    _mutex.lock();
    _timers.push_back(timer);
    _mutex.unlock();
}

void MessageHandler::removeTimer(int id){
    _mutex.lock();
    _timers.erase(std::remove_if(_timers.begin(), _timers.end(),[id](Timer t) { return (t.id == id); }), _timers.end());
    _mutex.unlock();
}

void MessageHandler::sendMessage(const SendMessage& message){
    _mutex.lock();
    _messageStack.push_back(message);
    _mutex.unlock();
    _waitForMessageMutex.unlock();
}

//Requires equal operator and merge function for SendMessage
void MessageHandler::mergeOrSendMessage(const SendMessage& message){
    _mutex.lock();
    auto it = std::find(_messageStack.begin(), _messageStack.end(), message);
    if(it==_messageStack.end()){
        _messageStack.push_back(message);
        _mutex.unlock();
        _waitForMessageMutex.unlock();
        return;
    }
    it->merge(message);
    _mutex.unlock();
}

//receiveMessage returns either a message from the stack or a timeoutMessage
//Waits for a message to appear on the stack or a timer to be reached
std::optional<SendMessage> MessageHandler::receiveMessage(){

    bool receivedMessage = false;
    std::vector<Timer>::iterator nextTimeoutPtr;
    std::chrono::steady_clock::time_point now;
    if(_timers.size() == 0){
        _waitForMessageMutex.lock();
        receivedMessage = true;
    }
    else{
        _mutex.lock();
        nextTimeoutPtr = std::min_element(_timers.begin(), _timers.end());
        _mutex.unlock();
        now = std::chrono::steady_clock::now();
        if(now < nextTimeoutPtr->timeoutTime){
            receivedMessage = _waitForMessageMutex.try_lock_until(nextTimeoutPtr->timeoutTime);
        }
    }

    if(receivedMessage){//waitForMessageMutex is locked
        _mutex.lock();
        if(_messageStack.size() == 0){
            _waitForMessageMutex.unlock();
            _mutex.unlock();
            return std::nullopt;
        }
        SendMessage message = _messageStack.back();
        _messageStack.pop_back();
        if(_messageStack.size() > 0){
            _waitForMessageMutex.unlock();
        }
        _mutex.unlock();
        return message;
    }

    //timeout (waitForMessageMutex is not locked)
    int timeouts = 1;
    if(nextTimeoutPtr->isRepeating){
        timeouts = 1 + (now-nextTimeoutPtr->timeoutTime)/nextTimeoutPtr->interval;
    }
    TimeoutMessage message;
    message.timerId = nextTimeoutPtr->id;
    message.timeouts = timeouts;
    SendMessage sendMessage;
    sendMessage.toId = nextTimeoutPtr->toId;
    sendMessage.message = message;

    _mutex.lock();
    auto it = std::find(_messageStack.begin(), _messageStack.end(), sendMessage);
    if(it!=_messageStack.end()){
        sendMessage.merge(*it);
        _messageStack.erase(it);
    }
    if(nextTimeoutPtr->isRepeating){
    nextTimeoutPtr->timeoutTime+=nextTimeoutPtr->interval*timeouts;
    }
    else{
        _timers.erase(nextTimeoutPtr);
    }
    _mutex.unlock();
    return sendMessage;
}