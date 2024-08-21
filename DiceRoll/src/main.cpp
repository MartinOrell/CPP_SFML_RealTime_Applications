#include "MessageManager.h"
#include "CapsuleRunner.h"
#include <memory>
#include <thread>
#include "Main_Capsule.h"
#include "Dice_Capsule.h"

int main(){

    int fps = 60;
    int diceRollSpeed = 100;
    int minDiceValue = 1;
    int maxDiceValue = 6;

    mrt::MessageManager messageManager;

    int nextCapsuleId = 0;

    mrt::CapsuleRunner capsuleRunner(nextCapsuleId++, &messageManager);
    mrt::CapsuleRunner timerRunner(nextCapsuleId++, &messageManager);
    auto main = std::make_unique<Main_Capsule>(nextCapsuleId++, &capsuleRunner, &timerRunner, fps);
    auto dice = std::make_unique<Dice_Capsule>(nextCapsuleId++, &capsuleRunner, &timerRunner, diceRollSpeed, minDiceValue, maxDiceValue);

    main->connect(dice->getId());

    capsuleRunner.addCapsule(std::move(main));
    capsuleRunner.addCapsule(std::move(dice));

    std::jthread timerThread = std::jthread([&timerRunner](){timerRunner.run();});
    capsuleRunner.run();
    timerRunner.stop();
}