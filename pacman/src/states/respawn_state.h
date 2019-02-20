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

    /* Prefix Message */
    std::string m_prefix_msg = "STARTING";

public:
    using State::State;

    RespawnState(GameContext context, float wait_seconds, std::string_view prefix_msg = "STARTING");

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};

}  // namespace pac
