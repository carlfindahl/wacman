#pragma once

#include "state.h"

namespace pac
{
class MainMenuState : public State
{
private:

public:
    MainMenuState() = default;

    explicit MainMenuState(StateManager& owner) : State(owner) {}

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};

}  // namespace pac
