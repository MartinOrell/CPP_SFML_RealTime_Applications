#pragma once

#include "Capsule.h"

#include <string>
#include <iostream>
#include "Message.h"
#include "SendMessage.h"
#include <chrono>
#include <stdexcept>
#include "Gui.h"

class CapsuleRunner;

class Main_Capsule: public Capsule{
    public:
        Main_Capsule(int id, CapsuleRunner* capsuleRunnerPtr, CapsuleRunner* timerRunnerPtr, int fps);
        int getId();
        void start();
        void handleMessage(Message message);
    
    private:

        void handleTimeout(TimeoutMessage message);
        void update(int timeouts);

        int _id;
        CapsuleRunner* _capsuleRunnerPtr;
        CapsuleRunner* _timerRunnerPtr;
        int _updateTimerId;
        std::chrono::steady_clock::duration _updateTime;
        Gui _gui;
        enum State{Running, End};
        State _state;
};