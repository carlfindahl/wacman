#pragma once

#include "state.h"
#include "rendering/renderer.h"

#include <vector>
#include <string>

namespace pac
{
class HighScoreState : public State
{
private:
    /*!
     * \brief The Entry struct represents a single entry in the high score table
     */
    struct Entry
    {
        std::string name = {};
        int score = 0;
    };

    /* Splash screen texture */
    TextureID m_splash_texture = {};

    /* All entries */
    std::vector<Entry> m_entries = {};

public:
    using State::State;

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;

private:
    /*!
     * \brief load_entries_from_file loads high score entries from highscores.txt in order to display them in the menu
     */
    void load_entries_from_file();
};

}  // namespace pac
