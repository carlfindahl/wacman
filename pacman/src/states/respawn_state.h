#pragma once

#include "state.h"

#include <chrono>

namespace pac
{
class RespawnState : public State
{
private:
    /* Respawn state is active for 3 seconds, then the game starts */
    std::chrono::duration<float> m_respawn_timer{3.f};

public:
    using State::State;

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};

}  // namespace pac
