#include "Timer.h"

using namespace mrt;

bool Timer::operator<(Timer timer){
    return timeoutTime < timer.timeoutTime;
}