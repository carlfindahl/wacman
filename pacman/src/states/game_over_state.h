#pragma once

#include "states/state.h"

#include <array>
#include <cstdint>

namespace pac
{
class GameOverState : public State
{
private:
    /* Name of player (going out) */
    std::array<char, 32> m_playername = {};

    /* Title of window */
    const char* m_title = {};

    /* Score given from game */
    int32_t m_score = 0u;

public:
    using State::State;

    GameOverState(GameContext context, int score, const char* title_text) : State(context), m_title(title_text), m_score(score) {}

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};
}  // namespace pac
