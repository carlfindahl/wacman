#pragma once

#include "state.h"
#include "common.h"
#include "rendering/renderer.h"

#include <vector>
#include <string>

namespace pac
{
class HighScoreState : public State
{
private:
    /* Splash screen texture */
    TextureID m_splash_texture = {};

    /* All entries */
    std::vector<ScoreEntry> m_entries = {};

public:
    using State::State;

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};

}  // namespace pac
