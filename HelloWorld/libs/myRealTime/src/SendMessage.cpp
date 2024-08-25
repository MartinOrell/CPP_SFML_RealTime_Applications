#include "SendMessage.h"

using namespace mrt;

bool SendMessage::operator==(const SendMessage& rhs){
    
    if(std::holds_alternative<TimeoutMessage>(message) &&
    std::holds_alternative<TimeoutMessage>(rhs.message)){
        return std::get<TimeoutMessage>(message).timerId ==
            std::get<TimeoutMessage>(rhs.message).timerId;
    }
    return false;
}

void SendMessage::merge(SendMessage fromM){
    if(std::holds_alternative<TimeoutMessage>(message) &&
    std::holds_alternative<TimeoutMessage>(fromM.message)){
        std::get<TimeoutMessage>(message).timeouts +=
            std::get<TimeoutMessage>(fromM.message).timeouts;
    } 
}