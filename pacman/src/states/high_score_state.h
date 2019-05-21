#pragma once

#include "state.h"
#include "common.h"
#include "rendering/renderer.h"

#include <vector>
#include <string>

#include <robinhood/robinhood.h>

namespace pac
{
class HighScoreState : public State
{
private:
    /* Splash screen texture */
    TextureID m_splash_texture = {};

    /* All high score entries */
    robin_hood::unordered_map<std::string, std::vector<ScoreEntry>> m_entries;

public:
    using State::State;

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;

private:
    /*!
     * \brief scores_for creates score UI for level with given name
     * \param level_name is name to show high scores for
     */
    void scores_for(const std::string& level_name);
};

}  // namespace pac
