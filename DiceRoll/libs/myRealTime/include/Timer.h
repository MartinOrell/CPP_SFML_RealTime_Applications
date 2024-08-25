#pragma once

#include <chrono>

namespace mrt{

    struct Timer{
        int id;
        int toId;
        std::chrono::steady_clock::time_point timeoutTime;
        bool isRepeating;
        std::chrono::steady_clock::duration interval;

        bool operator<(Timer);
    };
    
}