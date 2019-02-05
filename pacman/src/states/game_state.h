#pragma once

#include <states/state.h>

namespace pac {
class GameState : public State
{
private:

public:
    void on_enter() override;

    void on_exit() override;

    void update(float dt) override;
};
} // namespace pac
