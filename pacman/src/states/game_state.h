#pragma once

#include <states/state.h>

namespace pac {
class GameState : public State
{
private:

public:
    GameState();

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};
} // namespace pac
