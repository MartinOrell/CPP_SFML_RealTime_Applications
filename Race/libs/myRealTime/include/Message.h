#pragma once

#include <variant>
#include "Timer.h"

namespace mrt{

    struct TimeoutMessage{
        int timerId;
        int timeouts;
    };

    struct CancelTimer{
        int id;
    };

    enum VoidMessage{EndMessage, StartSignal, DistanceRequest};

    struct DistanceResponse{
        int fromId;
        int dist;
    };

    struct GoalReached{
        int fromId;
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