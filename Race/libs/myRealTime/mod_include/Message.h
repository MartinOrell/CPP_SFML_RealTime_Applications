#pragma once

#include <variant>
#include "Timer.h"

namespace mrt{

    struct TimeoutMessage{
        int timerId;
        int timeouts;
    };

    struct CancelTimer{
        int timerId;
    };

    enum VoidMessage{EndMessage, StartSignal, DistanceRequest, ClickMessage};

    struct DistanceResponse{
        int fromId;
        int dist;
    };

    struct GoalReached{
        int racerId;
    };

    typedef std::variant<
        TimeoutMessage,
        Timer, //defined in Timer.h
        CancelTimer,
        VoidMessage,
        DistanceResponse,
        GoalReached
    > Message;

}