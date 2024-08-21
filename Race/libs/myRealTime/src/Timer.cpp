#include "Timer.h"

using namespace mrt;

bool Timer::operator<(Timer a){
    return timeoutTime < a.timeoutTime;
}