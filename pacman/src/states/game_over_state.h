#pragma once

#include "states/state.h"

#include <array>
#include <cstdint>

namespace pac
{
class GameOverState : public State
{
private:
    std::array<char, 32> m_playername = {};

    int32_t m_score = 0u;

public:
    using State::State;

    GameOverState(GameContext context, int score) : State(context), m_score(score) {}

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};
}  // namespace pac
