#include "MessageManager.h"

#include "CapsuleRunner.h"

#include <string>
#include <stdexcept>

using namespace mrt;

MessageManager::MessageManager(bool allowCrossInvoke){
    _allowCrossInvoke = allowCrossInvoke; //allowing invoke through other capsuleRunners might not be thread safe
}

void MessageManager::addCapsuleRunnerPtr(CapsuleRunner* capsuleRunner){
    _capsuleRunnerPtrs.push_back(capsuleRunner);
}

void MessageManager::replaceCapsuleRunnerPtr(CapsuleRunner* to, CapsuleRunner* from){
    for(auto& ptr : _capsuleRunnerPtrs){
        if(ptr == from){
            ptr = to;
            return;
        }
    }
    throw std::runtime_error("MessageManager failed to find pointer during replacement");
}

void MessageManager::sendMessage(const SendMessage& sendMessage){
    for(auto it = _capsuleRunnerPtrs.begin(); it < _capsuleRunnerPtrs.end(); it++){
        if((*it)->isResponsible(sendMessage.toId)){
            (*it)->sendMessage(sendMessage);
            return;
        }
    }
    std::string errorMsg =
        "MessageManager unable to send message to Capsule[" +
        std::to_string(sendMessage.toId) +
        "]";
    throw std::invalid_argument(errorMsg);
}

void MessageManager::mergeOrSendMessage(const SendMessage& sendMessage){
    for(auto it = _capsuleRunnerPtrs.begin(); it < _capsuleRunnerPtrs.end(); it++){
        if((*it)->isResponsible(sendMessage.toId)){
            (*it)->mergeOrSendMessage(sendMessage);
            return;
        }
    }
    std::string errorMsg =
        "MessageManager unable to mergeOrSend message to Capsule[" +
        std::to_string(sendMessage.toId) +
        "]";
    throw std::invalid_argument(errorMsg);
}

Message MessageManager::invokeMessage(const SendMessage& request){
    if(!_allowCrossInvoke){
        throw std::invalid_argument("Invoking messages across capsuleRunners is not allowed (can be set to allowed in MessageManager, but this might not be thread safe)");
    }
    for(auto it = _capsuleRunnerPtrs.begin(); it < _capsuleRunnerPtrs.end(); it++){
        if((*it)->isResponsible(request.toId)){
            return (*it)->invokeMessage(request);
        }
    }
    std::string errorMsg =
        "MessageManager unable to invoke message to Capsule[" +
        std::to_string(request.toId) +
        "]";
    throw std::invalid_argument(errorMsg);

}