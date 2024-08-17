#include "MessageManager.h"

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

void MessageManager::sendMessage(SendMessage sendMessage){
    for(auto it = _capsuleRunnerPtrs.begin(); it < _capsuleRunnerPtrs.end(); it++){
        if((*it)->isResponsible(sendMessage.toId)){
            (*it)->sendMessage(sendMessage);
            return;
        }
    }
    throw std::invalid_argument("MessageManager unable to send message to id: " + std::to_string(sendMessage.toId));
}

void MessageManager::mergeOrSendMessage(SendMessage sendMessage){
    for(auto it = _capsuleRunnerPtrs.begin(); it < _capsuleRunnerPtrs.end(); it++){
        if((*it)->isResponsible(sendMessage.toId)){
            (*it)->mergeOrSendMessage(sendMessage);
            return;
        }
    }
    throw std::invalid_argument("MessageManager unable to send message to id: " + std::to_string(sendMessage.toId));

}

Message MessageManager::invokeMessage(SendMessage request){
    if(!_allowCrossInvoke){
        throw std::invalid_argument("Invoking messages across capsuleRunners is not allowed (can be set to allowed in MessageManager)");
    }
    for(auto it = _capsuleRunnerPtrs.begin(); it < _capsuleRunnerPtrs.end(); it++){
        if((*it)->isResponsible(request.toId)){
            return (*it)->invokeMessage(request);
        }
    }
    throw std::invalid_argument("MessageManager unable to invoke message to id: " + std::to_string(request.toId));

}