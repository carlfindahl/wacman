#pragma once

#include "state.h"

namespace pac
{
class PauseState : public State
{
public:
    PauseState() = default;

    explicit PauseState(StateManager& owner) : State(owner) {}

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};

}  // namespace pac
