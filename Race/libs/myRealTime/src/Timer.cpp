#include "Timer.h"

bool Timer::operator<(Timer a){
    return timeoutTime < a.timeoutTime;
}