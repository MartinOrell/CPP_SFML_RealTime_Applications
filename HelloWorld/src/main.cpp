#include "MessageManager.h"
#include "CapsuleRunner.h"
#include <memory>
#include <thread>
#include <vector>
#include "Main_Capsule.h"

int main(){

    int fps = 60;

    MessageManager messageManager;

    int nextCapsuleId = 0;

    CapsuleRunner capsuleRunner(nextCapsuleId++, &messageManager);
    CapsuleRunner timerRunner(nextCapsuleId++, &messageManager);
    auto main = std::make_unique<Main_Capsule>(nextCapsuleId++, &capsuleRunner, &timerRunner, fps);

    capsuleRunner.addCapsule(std::move(main));

    std::jthread timerThread = std::jthread([&timerRunner](){timerRunner.run();});
    capsuleRunner.run();
    timerRunner.stop();
}