#pragma once

#include <string>
#include <chrono>

struct RacerProfile{
    std::string name;
    std::string asciiFilename;
    int speed;
    int stamina;
    std::chrono::steady_clock::duration reactionTime;
    std::chrono::steady_clock::duration restTime;
};